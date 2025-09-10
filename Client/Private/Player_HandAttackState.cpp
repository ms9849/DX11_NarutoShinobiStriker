#include "Player_HandAttackState.h"

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

/*
·¹ÇÁÆ® ÆÝÄ¡ ->
½ºÆ®·¹ÀÌÆ® ->
´ÏÅ± ->
ÀèÄÆ ->
µå·¡°ï Å± 

¼øÀ¸·Î ÁøÇà 
*/

CPlayer_HandAttackState::CPlayer_HandAttackState(CPlayer* pPlayer)
    : m_pPlayer { pPlayer }
{
    Safe_AddRef(m_pPlayer);
}

void CPlayer_HandAttackState::Start(_bool IsBlend)
{
    m_pPlayer->Set_AnimIndex("CustomMan_Attack_Hand_Punch_Left", 2.5f, true);
    m_eAnimState = ANIM_STATE::LEFT_PUNCH;
}

CPlayerState* CPlayer_HandAttackState::Update(_float fTimeDelta)
{
    CPlayerState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPlayer->Get_AnimProgress();

    if (false == IsAnimFinished && fAnimProgress <= 0.7f)
        m_pPlayer->Get_Transform()->Go_Straight(fTimeDelta * m_pGameInstance->Calc_Quadratic(-0.89f, 0.48f, 0.11f, fAnimProgress));

    if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON) 
        && m_eAnimState == ANIM_STATE::LEFT_PUNCH && fAnimProgress >= 0.5f)
    {
        m_pPlayer->Set_AnimIndex("CustomMan_Attack_Hand_StraightPunch", 5.f, true);
        m_eAnimState = ANIM_STATE::STRAIGHT_PUNCH;
    }

    else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON)
        && m_eAnimState == ANIM_STATE::STRAIGHT_PUNCH && fAnimProgress >= 0.5f)
    {
        m_pPlayer->Set_AnimIndex("CustomMan_Attack_Hand_KneeKickUp", 3.f, false);
        m_eAnimState = ANIM_STATE::KNEE_KICK;
    }

    else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON)
        && m_eAnimState == ANIM_STATE::KNEE_KICK && fAnimProgress >= 0.5f)
    {
        m_pPlayer->Set_AnimIndex("CustomMan_Attack_Hand_JackCut", 2.5f, false);
        m_eAnimState = ANIM_STATE::JACK_CUT;
    }

    else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON)
        && m_eAnimState == ANIM_STATE::JACK_CUT && fAnimProgress >= 0.5f)
    {
        m_pPlayer->Set_AnimIndex("CustomMan_Attack_Hand_DragonKick", 3.f, true);
        m_eAnimState = ANIM_STATE::DRAGON_KICK;
    }

    else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON)
        && m_eAnimState == ANIM_STATE::DRAGON_KICK && fAnimProgress >= 0.5f)
    {
        m_pPlayer->Set_AnimIndex("CustomMan_Attack_Hand_Punch_Left", 2.5f, true);
        m_eAnimState = ANIM_STATE::LEFT_PUNCH;
    }

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
        {
            pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::RIGHT);
        }
    }
    // ¿ÞÂÊ ½ºÅÜ
    else if (m_pGameInstance->Key_Pressing(DIK_A)
        && fAnimProgress >= 0.2f
        && nullptr == pNextState)
    {
        if (m_pGameInstance->Key_Down(DIK_LSHIFT) && nullptr == pNextState)
        {
            pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::LEFT);
        }
    }


    return pNextState;
}

_bool CPlayer_HandAttackState::End()
{
    return true;
}

CPlayer_HandAttackState* CPlayer_HandAttackState::Create(CPlayer* pPlayer)
{
    return new CPlayer_HandAttackState(pPlayer);
}

void CPlayer_HandAttackState::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
}
