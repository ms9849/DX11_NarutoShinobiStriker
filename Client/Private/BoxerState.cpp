#include "BoxerState.h"

#include "GameInstance.h"
#include "GameManager.h"

CBoxerState::CBoxerState()
	: m_pGameInstance { CGameInstance::GetInstance() }
	, m_pGameManager { CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pGameManager);
}

void CBoxerState::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pGameManager);
}
