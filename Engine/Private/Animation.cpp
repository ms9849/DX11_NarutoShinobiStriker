#include "Animation.h"

#include "Channel.h"
#include "Bone.h"
#include "Model.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

/* 채널만 얕은 복사로 쓴다. */
CAnimation::CAnimation(const CAnimation& rhs)
	: m_fCurrentTrackPosition{ rhs.m_fCurrentTrackPosition }
	, m_fDuration{ rhs.m_fDuration }
	, m_fTickPerSecond{ rhs.m_fTickPerSecond }
	, m_iNumChannels{ rhs.m_iNumChannels }
	, m_Channels{ rhs.m_Channels }
	, m_CurrentKeyFrameIndices{ rhs.m_CurrentKeyFrameIndices }
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);

	strcpy_s(m_szName, rhs.m_szName);
}

_float CAnimation::Get_CurrentTrackPosition()
{
	//트랙포지션이 0이였다면, 가장 최대 트랙포지션과도 같을 것
	if (0.f == m_fCurrentTrackPosition)
		return m_fDuration;

	return m_fCurrentTrackPosition;
}

void CAnimation::Set_AnimProgress(_float fProgress)
{
	m_fCurrentTrackPosition = fProgress * m_fDuration;
}

_float CAnimation::Get_AnimProgress()
{
	return m_fCurrentTrackPosition / m_fDuration;
}

void CAnimation::Reset_Animation()
{
	m_fBlendRatio = 0.f;
	m_fCurrentTrackPosition = 0;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_CurrentKeyFrameIndices[i] = 0;
	}
}

/* TrackPosition에 해당하는 뼈 인덱스 & 키프레임 반환 */
map<_int, KEYFRAME> CAnimation::Get_KeyFrames(_float fTrackPoistion)
{
	map<_int, KEYFRAME> KeyFrames;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		_int iBoneIndex = m_Channels[i]->Get_BoneIndex();
		KEYFRAME KeyFrame = m_Channels[i]->Get_KeyFrame(fTrackPoistion);

		KeyFrames.emplace(iBoneIndex, KeyFrame);
	}

	return KeyFrames;
}

HRESULT CAnimation::Initialize(const class CModel* pModel, const aiAnimation* pAIAnimation)
{
	_char pSour[1024];
	_char* pResult{};
	_char* pTemp {};
	_bool isSeperated = { false };
	
	/* 이름 | 기준으로 잘라주기. 블렌더용 처리.. */
	strcpy_s(pSour, pAIAnimation->mName.data);
	pResult = strtok_s(pSour, "|", &pTemp);

	while (0 != strcmp(pTemp, ""))
	{
		pResult = strtok_s(NULL, "|", &pTemp);
		isSeperated = true;
	}

	if (isSeperated)
		strcpy_s(m_szName, pResult);

	else
		strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fDuration = (_float)pAIAnimation->mDuration;
	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	m_iNumChannels = pAIAnimation->mNumChannels;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pModel, pAIAnimation->mChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::Initialize(HANDLE hHandle, DWORD* dwByte)
{
	if (FAILED(Load_Animation_FromBinary(hHandle, dwByte)))
		return E_FAIL;

	return S_OK;
}


_bool CAnimation::Update_TransformationMatrices(const vector<CBone*>& Bones, _bool isLoop, _float fTimeDelta)
{
	/* 내 애니메이션의 현재 재생위치. */
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		Reset_Animation();
		return true;
	}

	_uint		iIndex = {};


	for (auto& pChannel : m_Channels)
		pChannel->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[iIndex++]);

	return false;
}

_bool CAnimation::Update_Blending_TransformationMatrices(_bool* bFlag, map<_int, KEYFRAME>* pPreKeyFrames, const vector<class CBone*>& Bones, _float PreTrackPosition, _float fBlendRatio, _float fTimeDelta)
{
	_uint		iIndex = {};

	//애니메이션 교체가 막 시작되는 시점.
	if (0.f == m_fBlendRatio)
	{
		for (auto& pChannel : m_Channels)
		{
			auto  iter = pPreKeyFrames->find(pChannel->Get_BoneIndex());

			if (pPreKeyFrames->end() == iter)
			{
				_int		iBoneIndex = pChannel->Get_BoneIndex();
				_matrix		BoneMatrix = Bones[iBoneIndex]->Get_TransformMatrix();
				KEYFRAME	SourKeyFrame;
				_vector		SourScale{}, SourRotation{}, SourTranslation{};

				/* 매칭 되는 본의 정보 다이렉트로 가져올 것 */
				XMMatrixDecompose(&SourScale, &SourRotation, &SourTranslation, BoneMatrix);

				XMStoreFloat3(&SourKeyFrame.vScale, SourScale);
				XMStoreFloat4(&SourKeyFrame.vRotation, SourRotation);
				XMStoreFloat3(&SourKeyFrame.vTranslation, SourTranslation);

				SourKeyFrame.fTrackPosition = PreTrackPosition;

				pPreKeyFrames->emplace(iBoneIndex, SourKeyFrame);
			}	
		}
	}

	// 블랜드 ratio 증가
	m_fBlendRatio += fBlendRatio;

	//선형보간 끝.
	if (m_fBlendRatio >= 1.0f)
		m_fBlendRatio = 1.f;

	for (auto& pChannel : m_Channels)
		pChannel->Update_Blending_TransformationMatrix(m_fBlendRatio, (*pPreKeyFrames)[pChannel->Get_BoneIndex()], Bones, PreTrackPosition);

	if (m_fBlendRatio == 1.0f)
	{
		m_fBlendRatio = 0.f;
		*bFlag = false;
	}

	return false;
}

HRESULT CAnimation::Save_Animation_ToBinary(HANDLE hHandle, DWORD* dwByte) const
{
	/* 
	1. 이름 
	2. 총 길이 ( m_fDuration )  
	3. 재생 속도 
	4. 채널 갯수 
	5. 채널 정보
	*/
	/* 애니메이션의 이름 저장 */
	if (false == WriteFile(hHandle, &m_szName, ANIM_MAX, dwByte, nullptr))
		return E_FAIL;
	/* 애니메이션의 총 길이 저장 */
	if (false == WriteFile(hHandle, &m_fDuration, sizeof(_float), dwByte, nullptr))
		return E_FAIL;
	/* 애니메이션의 속도 저장*/
	if (false == WriteFile(hHandle, &m_fTickPerSecond, sizeof(_float), dwByte, nullptr))
		return E_FAIL;
	/* 애니메이션의 채널갯수 저장 */
	if (false == WriteFile(hHandle, &m_iNumChannels, sizeof(_uint), dwByte, nullptr))
		return E_FAIL;
	/* 애니메이션 전체 저장 */
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		if (FAILED(m_Channels[i]->Save_Channel_ToBinary(hHandle, dwByte)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CAnimation::Load_Animation_FromBinary(HANDLE hHandle, DWORD* dwByte)
{
	/* 애니메이션의 이름 로딩 */
	if (false == ReadFile(hHandle, &m_szName, ANIM_MAX, dwByte, nullptr))
		return E_FAIL;
	/* 애니메이션의 총 길이 로딩 */
	if (false == ReadFile(hHandle, &m_fDuration, sizeof(_float), dwByte, nullptr))
		return E_FAIL;
	/* 애니메이션의 속도 로딩*/
	if (false == ReadFile(hHandle, &m_fTickPerSecond, sizeof(_float), dwByte, nullptr))
		return E_FAIL;
	/* 애니메이션의 채널갯수 로딩 */
	if (false == ReadFile(hHandle, &m_iNumChannels, sizeof(_uint), dwByte, nullptr))
		return E_FAIL;
	/* 채널 갯수 받아왔으니까 여기서 다시 세팅해주기. */
	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	/* 애니메이션 전체 로딩 */
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(hHandle, dwByte);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}


CAnimation* CAnimation::Create(const CModel* pModel, const aiAnimation* pAIAnimation)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pModel, pAIAnimation)))
	{
		MSG_BOX("Create Failed : CAnimation By Assimp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Create(HANDLE hHandle, DWORD* dwByte)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(hHandle, dwByte)))
	{
		MSG_BOX("Create Failed : CAnimation By Binary");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();

	m_CurrentKeyFrameIndices.clear();
}