#include "BossState.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"

CBossState::CBossState()
    : m_pGameInstance{ CGameInstance::GetInstance() }
    , m_pGameManager{ CGameManager::GetInstance() }
    , m_pPlayerTransformCom{ m_pGameManager->Get_PlayerPtr()->Get_Transform() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pPlayerTransformCom);
    Safe_AddRef(m_pGameManager);
}



void CBossState::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pGameManager);
    Safe_Release(m_pPlayerTransformCom);
}
