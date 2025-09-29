#include "Pajama_SlidingKunaiState.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Player.h"
#include "Pajama.h"
#include "Kunai.h"

#pragma region TRANSFER_STATE

#include "Pajama_IdleState.h"

#pragma endregion

CPajama_SlidingKunaiState::CPajama_SlidingKunaiState(CNavigation* pNavigation, CPajama* pPajama, ANIM_STATE eAnimState)
    : m_pNavigationCom{ pNavigation }
    , m_pPajama{ pPajama }
    , m_eAnimState { eAnimState }
{
    Safe_AddRef(m_pNavigationCom);
}

void CPajama_SlidingKunaiState::Start(_bool IsBlend)
{
    if(ANIM_STATE::LEFT == m_eAnimState)
        m_pPajama->Set_AnimIndex("CustomMan_Throw_Aerial_Kunai_Left", 2.f, IsBlend, 0.1f);
    else if(ANIM_STATE::RIGHT == m_eAnimState)
        m_pPajama->Set_AnimIndex("CustomMan_Throw_Aerial_Kunai_Right", 2.f, IsBlend, 0.1f);

    m_pPajama->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
}

CPajamaState* CPajama_SlidingKunaiState::Update(_float fTimeDelta)
{
    CPajamaState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pPajama->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPajama->Get_AnimProgress();

    _float fStepSpeed = m_pGameInstance->Calc_Quadratic(-5.f, 4.f, 1.f, fAnimProgress);

    if (ANIM_STATE::LEFT == m_eAnimState)
        m_pPajama->Get_Transform()->Go_Left(fTimeDelta * fStepSpeed * 0.6f, m_pNavigationCom);

    else if (ANIM_STATE::RIGHT == m_eAnimState)
        m_pPajama->Get_Transform()->Go_Right(fTimeDelta * fStepSpeed * 0.6f, m_pNavigationCom);

    if (fAnimProgress >= 0.5f && false == m_IsKunaiThrow)
    {
        CKunai::KUNAI_DESC Desc;

        XMStoreFloat3(&Desc.vPosition, m_pPajama->Get_Transform()->Get_State(STATE::POSITION));
        XMStoreFloat3(&Desc.vDirection, m_pPlayerTransformCom->Get_State(STATE::POSITION) - m_pPajama->Get_Transform()->Get_State(STATE::POSITION));
        Desc.fSpeedPerSec = 20.f;

        m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Kunai"),
            ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Layer_Skill"), &Desc);

        m_IsKunaiThrow = true;
    }

    if (true == IsAnimFinished)
    {
        pNextState = CPajama_IdleState::Create(m_pNavigationCom, m_pPajama);
    }

    return pNextState;
}

_bool CPajama_SlidingKunaiState::End()
{
    return true;
}

CPajama_SlidingKunaiState* CPajama_SlidingKunaiState::Create(CNavigation* pNavigation, CPajama* pPajama, ANIM_STATE eAnimState)
{
    return new CPajama_SlidingKunaiState(pNavigation, pPajama, eAnimState);
}

void CPajama_SlidingKunaiState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
