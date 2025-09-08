#include "WhiteJetsuState.h"

#include "GameInstance.h"

CWhiteJetsuState::CWhiteJetsuState()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

void CWhiteJetsuState::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);

}
