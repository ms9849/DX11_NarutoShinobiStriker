#include "BirdState.h"

#include "GameInstance.h"

CBirdState::CBirdState()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

void CBirdState::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
