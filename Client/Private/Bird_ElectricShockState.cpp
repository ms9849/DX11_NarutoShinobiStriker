#include "Bird_ElectricShockState.h"

#include "Bird.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Bird_IdleState.h"

#pragma endregion

CBird_ElectricShockState::CBird_ElectricShockState(CNavigation* pNavigation, CBird* pBird)
    : m_pBird { pBird }
    , m_pNavigationCom{ pNavigation }
    , m_pGameManager{ CGameManager::GetInstance() }
{
    Safe_AddRef(m_pNavigationCom);
    Safe_AddRef(m_pGameManager);

}

void CBird_ElectricShockState::Start(_bool IsBlend)
{
    m_pBird->Set_AnimIndex("TenTailsCloneLoser02_Beaten_ElectricShock_Loop", 2.f, true);
}

CBirdState* CBird_ElectricShockState::Update(_float fTimeDelta)
{
    CBirdState* pNextState = { nullptr };
    _bool  IsAnimFinished = m_pBird->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pBird->Get_AnimProgress();

    m_fTimeAcc += fTimeDelta;

    if (m_fMaxTimeAcc <= m_fTimeAcc)
    {
        pNextState = CBird_IdleState::Create(m_pNavigationCom, m_pBird);
    }

    return pNextState;
}

_bool CBird_ElectricShockState::End()
{
    return true;
}

CBird_ElectricShockState* CBird_ElectricShockState::Create(CNavigation* pNavigation, CBird* pBird)
{
    return new CBird_ElectricShockState(pNavigation, pBird);
}

void CBird_ElectricShockState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pGameManager);
}
