#include "BoxerState.h"

#include "GameInstance.h"

CBoxerState::CBoxerState()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

void CBoxerState::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
