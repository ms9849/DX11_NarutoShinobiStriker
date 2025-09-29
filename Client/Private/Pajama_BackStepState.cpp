#include "Pajama_BackStepState.h"
#include "GameManager.h"
#include "GameInstance.h"

#include "Player.h"
#include "Pajama.h"

#pragma region TRANSFER_STATE

#include "Pajama_IdleState.h"

#pragma endregion

CPajama_BackStepState::CPajama_BackStepState(CNavigation* pNavigation, CPajama* pPajama)
    : m_pNavigationCom{ pNavigation }
    , m_pPajama{ pPajama }
{
    Safe_AddRef(m_pNavigationCom);
}

void CPajama_BackStepState::Start(_bool IsBlend)
{
    m_pPajama->Set_AnimIndex("CustomMan_DashStep_Behind", 1.4f, IsBlend, 0.1f);
}

CPajamaState* CPajama_BackStepState::Update(_float fTimeDelta)
{
    CPajamaState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pPajama->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPajama->Get_AnimProgress();

    _float fStepSpeed = m_pGameInstance->Calc_Quadratic(-7.86f, 3.57f, 1.f, fAnimProgress);

    if (fAnimProgress != 0.f && fAnimProgress <= 0.6f)
        m_pPajama->Get_Transform()->Go_Backward(fTimeDelta * fStepSpeed, m_pNavigationCom);

    if (0.65f <= fAnimProgress)
        pNextState = CPajama_IdleState::Create(m_pNavigationCom, m_pPajama);

    return pNextState;
}

_bool CPajama_BackStepState::End()
{
    return true;
}

CPajama_BackStepState* CPajama_BackStepState::Create(CNavigation* pNavigation, CPajama* pPajama)
{
    return new CPajama_BackStepState(pNavigation, pPajama);
}

void CPajama_BackStepState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
