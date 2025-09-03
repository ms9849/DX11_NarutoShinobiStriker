#include "GameManager.h"

#include "GameInstance.h"

#include "Camera.h"
#include "Player.h"
#include "QuestLog.h"

IMPLEMENT_SINGLETON(CGameManager);

CGameManager::CGameManager() 
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CGameManager::Initialize_GameManager()
{
	return S_OK;
}

void CGameManager::Release_GameManager()
{
	DestroyInstance();

	if(nullptr != m_pPlayer)
		m_pPlayer->Clear_State();

	Safe_Release(m_pPlayer);
	Safe_Release(m_pQuestLog);
	Safe_Release(m_pGameInstance);

	for (_uint i = 0; i < ENUM_CLASS(LEVEL::END); ++i)
	{
		for (auto& pCamera : m_Cameras[i])
			Safe_Release(pCamera.second);

		m_Cameras[i].clear();
	}
}

void CGameManager::Clear()
{
	/* 지울때 레퍼런스 카운트가 0임을 보장하지 않음*/
	/* 따라서 명시적으로 nullptr 처리 해줘야함 */

	Safe_Release(m_pPlayer);
	if (nullptr != m_pPlayer)
	{
		m_pPlayer->Clear_State();
		m_pPlayer = nullptr;
	}

	Safe_Release(m_pQuestLog);
	m_pQuestLog = nullptr;

	for (_uint i = 0; i < ENUM_CLASS(LEVEL::END); ++i)
	{
		for (auto& pCamera : m_Cameras[i])
			Safe_Release(pCamera.second);

		m_Cameras[i].clear();
	}

	m_pActivatedCamera = nullptr;
}

HRESULT CGameManager::Set_PlayerPtr(CPlayer* pPlayer)
{
	//인자로 받은 플레이어가 nullptr 이라면
	if (nullptr == pPlayer)
		return E_FAIL;

	//이미 플레이어가 등록되어 있다면,
	if (nullptr != m_pPlayer)
		Safe_Release(m_pPlayer);

	m_pPlayer = pPlayer;
	Safe_AddRef(m_pPlayer);

	return S_OK;
}

HRESULT CGameManager::Set_QuestPtr(CQuestLog* pQuestLog)
{
	//인자로 받은 퀘스트가 nullptr 이라면, 
	// 그리고 이미 퀘스트가 등록되어 있다면.. 근데 퀘스트는 그럴 일 없다.
	if (nullptr == pQuestLog || nullptr != m_pQuestLog)
		return E_FAIL;

	m_pQuestLog = pQuestLog;
	Safe_AddRef(m_pQuestLog);

	return S_OK;
}

HRESULT CGameManager::Set_NextLevelID(LEVEL eLevelID)
{
	m_eNextLevel = eLevelID;

	return S_OK;
}

HRESULT CGameManager::Add_Camera(LEVEL eLevelID, const _wstring& strCameraTag, CCamera* pCamera)
{
	/* 이미 카메라가 존재하면 FAIL 반환. */
	auto iter = m_Cameras[ENUM_CLASS(eLevelID)].find(strCameraTag);

	if (iter != m_Cameras[ENUM_CLASS(eLevelID)].end())
		return E_FAIL;

	m_Cameras[ENUM_CLASS(eLevelID)].emplace(strCameraTag, pCamera);

	return S_OK;
}

HRESULT CGameManager::Change_Camera(LEVEL eLevelID, const _wstring& strCameraTag)
{
	/* 태그에 해당하는 카메라가 존재하지 않으면 FAIL 반환. */
	auto iter = m_Cameras[ENUM_CLASS(eLevelID)].find(strCameraTag);
	
	if (iter == m_Cameras[ENUM_CLASS(eLevelID)].end())
		return E_FAIL;

	/* 기존 카메라 Set Dead 세팅. 오브젝트 매니저에서 빠져 나오게 된다. */
	if(nullptr != m_pActivatedCamera)
		m_pActivatedCamera->Set_Dead(true);

	static_cast<CGameObject*>(iter->second)->Set_Dead(false);
	m_pGameInstance->Add_Clone_ToLayer(iter->second, ENUM_CLASS(eLevelID), TEXT("Layer_Camera"));

	/* 활성화중인 카메라 교체 */
	m_pActivatedCamera = iter->second;
	Safe_AddRef(m_pActivatedCamera);

	return S_OK;
}

LEVEL CGameManager::Get_NextLevel()
{
	if (m_pGameInstance->Get_LevelID() != ENUM_CLASS(LEVEL::LOADING))
		return LEVEL::END;

	return m_eNextLevel;
}

void CGameManager::Free()
{
	__super::Free();
}
