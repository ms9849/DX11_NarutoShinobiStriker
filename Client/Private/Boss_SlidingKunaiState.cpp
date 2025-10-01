#include "Boss_SlidingKunaiState.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Player.h"
#include "Boss.h"
#include "Kunai.h"

#pragma region TRANSFER_STATE
#include "Boss_IdleState.h"
#pragma endregion

CBoss_SlidingKunaiState::CBoss_SlidingKunaiState(CNavigation* pNavigation, CBoss* pBoss, ANIM_STATE eAnimState)
	: m_pNavigationCom{ pNavigation }
	, m_pBoss { pBoss }
	, m_eAnimState{ eAnimState }
{
	Safe_AddRef(m_pNavigationCom);
}

void CBoss_SlidingKunaiState::Start(_bool IsBlend)
{
	if (ANIM_STATE::LEFT == m_eAnimState)
		m_pBoss->Set_AnimIndex("CustomMan_Throw_Aerial_Kunai_Left", 2.f, IsBlend, 0.1f);
	else if (ANIM_STATE::RIGHT == m_eAnimState)
        m_pBoss->Set_AnimIndex("CustomMan_Throw_Aerial_Kunai_Right", 2.f, IsBlend, 0.1f);

    m_pBoss->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
}

CBossState* CBoss_SlidingKunaiState::Update(_float fTimeDelta)
{
    CBossState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pBoss->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pBoss->Get_AnimProgress();

    _float fStepSpeed = m_pGameInstance->Calc_Quadratic(-5.f, 4.f, 1.f, fAnimProgress);

    if (ANIM_STATE::LEFT == m_eAnimState)
        m_pBoss->Get_Transform()->Go_Left(fTimeDelta * fStepSpeed * 0.6f, m_pNavigationCom);

    else if (ANIM_STATE::RIGHT == m_eAnimState)
        m_pBoss->Get_Transform()->Go_Right(fTimeDelta * fStepSpeed * 0.6f, m_pNavigationCom);

    if (fAnimProgress >= 0.5f && false == m_IsKunaiThrow)
    {
        CKunai::KUNAI_DESC Desc;

        XMStoreFloat3(&Desc.vPosition, m_pBoss->Get_Transform()->Get_State(STATE::POSITION));
        XMStoreFloat3(&Desc.vDirection, m_pPlayerTransformCom->Get_State(STATE::POSITION) - m_pBoss->Get_Transform()->Get_State(STATE::POSITION));
        Desc.fSpeedPerSec = 20.f;

        m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Kunai"),
            ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Layer_Skill"), &Desc);

        m_IsKunaiThrow = true;
    }

    if (true == IsAnimFinished)
    {
        pNextState = CBoss_IdleState::Create(m_pNavigationCom, m_pBoss);
    }

    return pNextState;
}

_bool CBoss_SlidingKunaiState::End()
{
    return true;
}

CBoss_SlidingKunaiState* CBoss_SlidingKunaiState::Create(CNavigation* pNavigation, CBoss* pBoss, ANIM_STATE eAnimState)
{
    return new CBoss_SlidingKunaiState(pNavigation, pBoss, eAnimState);
}

void CBoss_SlidingKunaiState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
