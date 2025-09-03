#include "PlayerState.h"

#include "GameInstance.h"

CPlayerState::CPlayerState() 
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

void CPlayerState::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
