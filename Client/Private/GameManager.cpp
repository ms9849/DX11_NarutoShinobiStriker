#include "GameManager.h"

#include "GameInstance.h"
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

	Safe_Release(m_pPlayer);
	Safe_Release(m_pQuestLog);
	Safe_Release(m_pGameInstance);
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
