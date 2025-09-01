#include "Animation.h"

#include "Channel.h"
#include "Bone.h"

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

HRESULT CAnimation::Initialize(const class CModel* pModel, const aiAnimation* pAIAnimation)
{
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

void CAnimation::Update_TransformationMatrices(const vector<CBone*>& Bones, _float fTimeDelta)
{
	/* 내 애니메이션의 현재 재생위치. */
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		m_fCurrentTrackPosition = 0.f;
	}

	_uint		iIndex = {};

	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[iIndex++]);
	}
}

HRESULT CAnimation::Save_Animation_ToBinary(HANDLE hHandle, DWORD* dwByte, const aiAnimation* pAIAnimation) const
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
		if (FAILED(m_Channels[i]->Save_Channel_ToBinary(hHandle,  dwByte, pAIAnimation->mChannels[i])))
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