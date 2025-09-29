#include "Pajama_ElectricShockState.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Pajama.h"

#pragma region TRANSFER_STATE

#include "Pajama_IdleState.h"

#pragma endregion

CPajama_ElectricShockState::CPajama_ElectricShockState(CNavigation* pNavigation, CPajama* pPajama)
    : m_pPajama{ pPajama }
    , m_pNavigationCom{ pNavigation }
{
    Safe_AddRef(m_pNavigationCom);
}

void CPajama_ElectricShockState::Start(_bool IsBlend)
{
    /* Beaten보다 훨씬 멀리 날아가야 함 */
    m_pPajama->Set_AnimIndex("CustomMan_Beaten_ElectricShock_Loop", 2.f, true);

    m_pPajama->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
}

CPajamaState* CPajama_ElectricShockState::Update(_float fTimeDelta)
{
    CPajamaState* pNextState = { nullptr };
    _bool  IsAnimFinished = m_pPajama->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPajama->Get_AnimProgress();

    m_fTimeAcc += fTimeDelta;

    if (m_fMaxTimeAcc <= m_fTimeAcc)
    {
        pNextState = CPajama_IdleState::Create(m_pNavigationCom, m_pPajama);
    }

    return pNextState;
}

_bool CPajama_ElectricShockState::End()
{
    return true;
}

CPajama_ElectricShockState* CPajama_ElectricShockState::Create(CNavigation* pNavigation, CPajama* pPajama)
{
    return new CPajama_ElectricShockState(pNavigation, pPajama);
}

void CPajama_ElectricShockState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
