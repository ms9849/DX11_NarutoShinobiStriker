#include "Pajama_HandAttackState.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Player.h"
#include "Pajama.h"
#include "Kunai.h"

#pragma region TRANSFER_STATE

#include "Pajama_BackStepState.h"

#pragma endregion

CPajama_HandAttackState::CPajama_HandAttackState(CNavigation* pNavigation, CPajama* pPajama)
	: m_pNavigationCom{ pNavigation }
	, m_pPajama{ pPajama }
{
	Safe_AddRef(m_pNavigationCom);
}

void CPajama_HandAttackState::Start(_bool IsBlend)
{
	m_pPajama->Set_AnimIndex("CustomMan_Attack_Hand_Punch_Left", 2.f, IsBlend, 0.1f);
	m_pPajama->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
    m_pPajama->Get_Collider(TEXT("Com_Collider_HandAttack"))->Set_Active(false);

    m_pGameInstance->PlaySoundOnce(TEXT("Pajama_HandAttack.wav"), CHANNELID::EFFECT, 0.45f);

}

CPajamaState* CPajama_HandAttackState::Update(_float fTimeDelta)
{
    CPajamaState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pPajama->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPajama->Get_AnimProgress();

    if (false == IsAnimFinished && fAnimProgress <= 0.4f)
        m_pPajama->Get_Transform()->Go_Straight(fTimeDelta * m_pGameInstance->Calc_Quadratic(-10.f, 4.f, 0.f, fAnimProgress),
            m_pNavigationCom);

    Update_Collider(fAnimProgress);

    if (true == IsAnimFinished)
    {
        /* 나중에 백스텝으로 변경 */
        pNextState = CPajama_BackStepState::Create(m_pNavigationCom, m_pPajama);
    }

    return pNextState;
}

_bool CPajama_HandAttackState::End()
{
    m_pPajama->Get_Collider(TEXT("Com_Collider_HandAttack"))->Set_Active(false);

    return true;
}

void CPajama_HandAttackState::Update_Collider(_float fAnimProgress)
{
    if (fAnimProgress <= 0.1f || fAnimProgress >= 0.3f)
        m_pPajama->Get_Collider(TEXT("Com_Collider_HandAttack"))->Set_Active(false);

    else if (false == m_IsOnCollider)
    {
        m_pPajama->Get_Collider(TEXT("Com_Collider_HandAttack"))->Set_Active(true);
        m_IsOnCollider = true;
    }
}

CPajama_HandAttackState* CPajama_HandAttackState::Create(CNavigation* pNavigation, CPajama* pPajama)
{
    return new CPajama_HandAttackState(pNavigation, pPajama);
}

void CPajama_HandAttackState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
