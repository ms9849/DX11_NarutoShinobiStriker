#include "Animation.h"

#include "Channel.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(const class CModel* pModel, const aiAnimation* pAIAnimation)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fDuration = pAIAnimation->mDuration;
	m_fTickPerSecond = pAIAnimation->mTicksPerSecond;

	m_iNumChannels = pAIAnimation->mNumChannels;

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pModel, pAIAnimation->mChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Update_TransformationMatrices(const vector<CBone*>& Bones, _float fTimeDelta)
{
	/* 내 애니메이션의 현재 재생위치. */
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition);
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
	if (false == WriteFile(hHandle, &m_szName, MAX_PATH, dwByte, nullptr))
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
	for (_int i = 0; i < m_iNumChannels; ++i)
	{
		if (FAILED(m_Channels[i]->Save_Channel_ToBinary(hHandle,  dwByte, pAIAnimation->mChannels[i])))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CAnimation::Load_Animation_ToBinary(HANDLE hHandle, DWORD* dwByte)
{

	return S_OK;
}

CAnimation* CAnimation::Create(const class CModel* pModel, const aiAnimation* pAIAnimation)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pModel, pAIAnimation)))
	{
		MSG_BOX("Create Failed : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}