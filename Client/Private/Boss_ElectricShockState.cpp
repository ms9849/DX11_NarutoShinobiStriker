#include "Boss_ElectricShockState.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Boss.h"

#pragma region TRANSFER_STATE

#include "Boss_IdleState.h"

#pragma endregion

CBoss_ElectricShockState::CBoss_ElectricShockState(CNavigation* pNavigation, CBoss* pBoss)
	: m_pBoss { pBoss}
	, m_pNavigationCom{ pNavigation }
{
	Safe_AddRef(m_pNavigationCom);
}

void CBoss_ElectricShockState::Start(_bool IsBlend)
{    /* Beaten보다 훨씬 멀리 날아가야 함 */
    m_pBoss->Set_AnimIndex("CustomMan_Beaten_ElectricShock_Loop", 2.f, true);
    m_pBoss->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
}

CBossState* CBoss_ElectricShockState::Update(_float fTimeDelta)
{
    CBossState* pNextState = { nullptr };
    _bool  IsAnimFinished = m_pBoss->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pBoss->Get_AnimProgress();

    m_fTimeAcc += fTimeDelta;

    if (m_fMaxTimeAcc <= m_fTimeAcc)
    {
        pNextState = CBoss_IdleState::Create(m_pNavigationCom, m_pBoss);
    }

    return pNextState;
}

_bool CBoss_ElectricShockState::End()
{
    return true;
}

CBoss_ElectricShockState* CBoss_ElectricShockState::Create(CNavigation* pNavigation, CBoss* pBoss)
{
    return new CBoss_ElectricShockState(pNavigation, pBoss);
}

void CBoss_ElectricShockState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
