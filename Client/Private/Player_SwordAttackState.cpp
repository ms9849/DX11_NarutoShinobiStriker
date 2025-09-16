#include "Player_SwordAttackState.h"

#include "Player.h"
#include "GameInstance.h"

/* ÀüÀÌ °¡´ÉÇÑ »óÅÂµé */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"
#include "Player_RunState.h"
#include "Player_JumpState.h"
#include "Player_FrontJumpState.h"
#include "Player_StepState.h"

#pragma endregion

CPlayer_SwordAttackState::CPlayer_SwordAttackState(CPlayer* pPlayer)
    : m_pPlayer { pPlayer }
{
    Safe_AddRef(m_pPlayer);
}

void CPlayer_SwordAttackState::Start(_bool IsBlend)
{
    m_pPlayer->Set_AnimIndex("CustomMan_Attack_SnakeSword_cmb_01", 2.f, true );
    m_eAnimState = ANIM_STATE::ATTACK_01;
}

CPlayerState* CPlayer_SwordAttackState::Update(_float fTimeDelta)
{
    CPlayerState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPlayer->Get_AnimProgress();

    if (false == IsAnimFinished && fAnimProgress <= 0.5f && ANIM_STATE::ATTACK_01 == m_eAnimState)
        m_pPlayer->Get_Transform()->Go_Straight(fTimeDelta * m_pGameInstance->Calc_Linear(-1.4f, 0.7f, fAnimProgress),
            m_pPlayer->Get_Navigation());

    else  if (false == IsAnimFinished && fAnimProgress <= 0.3f && ANIM_STATE::ATTACK_02 == m_eAnimState)
        m_pPlayer->Get_Transform()->Go_Straight(fTimeDelta * m_pGameInstance->Calc_Linear(-2.3f, 0.7f, fAnimProgress),
            m_pPlayer->Get_Navigation());

    else if (false == IsAnimFinished && fAnimProgress <= 0.35f && fAnimProgress >= 0.25f && ANIM_STATE::ATTACK_03 == m_eAnimState)
        m_pPlayer->Get_Transform()->Go_Straight(fTimeDelta * m_pGameInstance->Calc_Linear(-4.f, 1.4f, fAnimProgress),
            m_pPlayer->Get_Navigation());


    if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON)
        && m_eAnimState == ANIM_STATE::ATTACK_01 && fAnimProgress >= 0.5f)
    {
        m_pPlayer->Set_AnimIndex("CustomMan_Attack_SnakeSword_cmb_02", 2.f, true);
        m_eAnimState = ANIM_STATE::ATTACK_02;
    }

    else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON)
        && m_eAnimState == ANIM_STATE::ATTACK_02 && fAnimProgress >= 0.5f)
    {
        m_pPlayer->Set_AnimIndex("CustomMan_Attack_SnakeSword_cmb_03", 2.f, true);
        m_eAnimState = ANIM_STATE::ATTACK_03;
    }


    // IDLE
    else if (true == IsAnimFinished)
    {
        pNextState = CPlayer_IdleState::Create(m_pPlayer);
    }

    // ¹é½ºÅÜ
    if (m_pGameInstance->Key_Pressing(DIK_S) && m_pGameInstance->Key_Down(DIK_LSHIFT)
        && fAnimProgress >= 0.2f
        && nullptr == pNextState)
    {
        pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::BACK);
        m_IsNextAnimBlened = true;
    }
    // ¿À¸¥ÂÊ ½ºÅÜ
    else if (m_pGameInstance->Key_Pressing(DIK_D)
        && fAnimProgress >= 0.2f
        && nullptr == pNextState)
    {
        if (m_pGameInstance->Key_Down(DIK_LSHIFT) && nullptr == pNextState)
            pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::RIGHT);
    }
    // ¿ÞÂÊ ½ºÅÜ
    else if (m_pGameInstance->Key_Pressing(DIK_A)
        && fAnimProgress >= 0.2f
        && nullptr == pNextState)
    {
        if (m_pGameInstance->Key_Down(DIK_LSHIFT) && nullptr == pNextState)
            pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::LEFT);
    }


    return pNextState;
}

_bool CPlayer_SwordAttackState::End()
{
    return true;
}

CPlayer_SwordAttackState* CPlayer_SwordAttackState::Create(CPlayer* pPlayer)
{
    return new CPlayer_SwordAttackState(pPlayer);
}

void CPlayer_SwordAttackState::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
}
