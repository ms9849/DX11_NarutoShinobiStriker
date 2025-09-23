#include "Player_HandAttackState.h"

#include "Player.h"
#include "GameInstance.h"
#include "GameManager.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"
#include "Player_RunState.h"
#include "Player_JumpState.h"
#include "Player_FrontJumpState.h"
#include "Player_StepState.h"

#pragma endregion

/*
레프트 펀치 ->
스트레이트 ->
니킥 ->
잭컷 ->
드래곤 킥 

순으로 진행 
*/

CPlayer_HandAttackState::CPlayer_HandAttackState(CPlayer* pPlayer)
    : m_pPlayer { pPlayer }
    , m_pGameManager{ CGameManager::GetInstance() }
{
    Safe_AddRef(m_pPlayer);
    Safe_AddRef(m_pGameManager);
}

void CPlayer_HandAttackState::Start(_bool IsBlend)
{
    m_pPlayer->Set_AnimIndex("CustomMan_Attack_Hand_Punch_Left", 2.5f, true);

    CTransform* pTargetTransform = m_pGameManager->Calc_Target(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));

    if (nullptr != pTargetTransform)
        m_pPlayer->Get_Transform()->LookAt_XZ(pTargetTransform->Get_State(STATE::POSITION));

    m_eAnimState = ANIM_STATE::LEFT_PUNCH;
}

CPlayerState* CPlayer_HandAttackState::Update(_float fTimeDelta)
{
    CTransform* pTargetTransform = m_pGameManager->Calc_Target(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));

    CPlayerState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPlayer->Get_AnimProgress();
    Update_Collider(fAnimProgress);

    if (m_eAnimState == ANIM_STATE::BACK_KICK && fAnimProgress >= 0.8f)
        m_pGameManager->Change_Camera(static_cast<LEVEL>(m_pGameInstance->Get_LevelID()), TEXT("Main_Camera"));

    if (false == IsAnimFinished && fAnimProgress <= 0.4f && ANIM_STATE::BACK_KICK != m_eAnimState)
        m_pPlayer->Get_Transform()->Go_Straight(fTimeDelta * m_pGameInstance->Calc_Quadratic(-10.f, 4.f, 0.f, fAnimProgress),
            m_pPlayer->Get_Navigation());
    else if(false == IsAnimFinished && fAnimProgress <= 0.3f && ANIM_STATE::BACK_KICK == m_eAnimState)
        m_pPlayer->Get_Transform()->Go_Straight(fTimeDelta * m_pGameInstance->Calc_Quadratic(-10.f, 4.f, 0.f, fAnimProgress),
            m_pPlayer->Get_Navigation());

    if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON) 
        && m_eAnimState == ANIM_STATE::LEFT_PUNCH && fAnimProgress >= 0.5f)
    {
        m_pPlayer->Set_AnimIndex("CustomMan_Attack_Hand_StraightPunch", 5.f, true);
        
        if (nullptr != pTargetTransform)
            m_pPlayer->Get_Transform()->LookAt_XZ(pTargetTransform->Get_State(STATE::POSITION));

        m_IsOnCollider = false;
        m_eAnimState = ANIM_STATE::STRAIGHT_PUNCH;
    }

    else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON)
        && m_eAnimState == ANIM_STATE::STRAIGHT_PUNCH && fAnimProgress >= 0.5f)
    {
        m_pPlayer->Set_AnimIndex("CustomMan_Attack_ChakraBoots_A_cmb_01", 1.25f, false);

        if (nullptr != pTargetTransform)
            m_pPlayer->Get_Transform()->LookAt_XZ(pTargetTransform->Get_State(STATE::POSITION));

        m_IsOnCollider = false;
        m_eAnimState = ANIM_STATE::KICK;
    }

    else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON)
        && m_eAnimState == ANIM_STATE::KICK && fAnimProgress >= 0.5f)
    {
        m_pGameManager->Change_Camera(static_cast<LEVEL>(m_pGameInstance->Get_LevelID()), TEXT("Action_Camera"));
        m_pPlayer->Set_AnimIndex("CustomMan_Attack_Hand_BackKick", 1.75f, false);

        if (nullptr != pTargetTransform)
            m_pPlayer->Get_Transform()->LookAt_XZ(pTargetTransform->Get_State(STATE::POSITION));

        m_IsOnCollider = false;
        m_eAnimState = ANIM_STATE::BACK_KICK;
    }

    else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON)
        && m_eAnimState == ANIM_STATE::BACK_KICK && fAnimProgress >= 0.8f)
    {
        m_pPlayer->Set_AnimIndex("CustomMan_Attack_Hand_Punch_Left", 2.5f, true);

        if (nullptr != pTargetTransform)
            m_pPlayer->Get_Transform()->LookAt_XZ(pTargetTransform->Get_State(STATE::POSITION));

        m_IsOnCollider = false;
        m_eAnimState = ANIM_STATE::LEFT_PUNCH;
    }

    else if (true == IsAnimFinished)
    {
        pNextState = CPlayer_IdleState::Create(m_pPlayer);
    }


    // 백스텝
    if (m_pGameInstance->Key_Pressing(DIK_S) && m_pGameInstance->Key_Down(DIK_LSHIFT)
        && fAnimProgress >= 0.2f 
        && nullptr == pNextState)
    {
        pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::BACK);
        m_IsNextAnimBlened = true;
    }
    // 오른쪽 스텝
    else if (m_pGameInstance->Key_Pressing(DIK_D)
        && fAnimProgress >= 0.2f
        && nullptr == pNextState)
    {
        if (m_pGameInstance->Key_Down(DIK_LSHIFT) && nullptr == pNextState)
        {
            pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::RIGHT);
        }
    }
    // 왼쪽 스텝
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
    m_pPlayer->Set_Collider_Active(TEXT("Com_Collider_HandAttack"), false);

    m_pGameManager->Change_Camera(static_cast<LEVEL>(m_pGameInstance->Get_LevelID()), TEXT("Main_Camera"));
    return true;
}

void CPlayer_HandAttackState::Update_Collider(_float fAnimProgress)
{
    /* 콜라이더 온/오프 */
    if(ANIM_STATE::BACK_KICK != m_eAnimState &&
        ANIM_STATE::STRAIGHT_PUNCH != m_eAnimState &&
        fAnimProgress <= 0.2f)
             m_pPlayer->Set_Collider_Active(TEXT("Com_Collider_HandAttack"), false);

    else if(ANIM_STATE::STRAIGHT_PUNCH == m_eAnimState &&
        fAnimProgress <= 0.3f)
        m_pPlayer->Set_Collider_Active(TEXT("Com_Collider_HandAttack"), false);

    else if (ANIM_STATE::BACK_KICK == m_eAnimState &&
        fAnimProgress <= 0.3f)
        m_pPlayer->Set_Collider_Active(TEXT("Com_Collider_HandAttack"), false);

    else if (m_IsOnCollider == false)
    {
        m_pPlayer->Set_Collider_Active(TEXT("Com_Collider_HandAttack"), true);
        m_IsOnCollider = true;
    }

    /* 콜라이더 세팅 */
    if (ANIM_STATE::BACK_KICK == m_eAnimState)
    {
        CGameManager::GetInstance()->Add_Collider_ToCollision(TEXT("Player_Attack"), COLLIDER_HANDLE_ID::PLAYER_HAND_ATTACK_FINAL,
            m_pPlayer->Get_Collider(TEXT("Com_Collider_HandAttack")));
    }
    else
    {
        CGameManager::GetInstance()->Add_Collider_ToCollision(TEXT("Player_Attack"), COLLIDER_HANDLE_ID::PLAYER_HAND_ATTACK,
            m_pPlayer->Get_Collider(TEXT("Com_Collider_HandAttack")));
    }
}

CPlayer_HandAttackState* CPlayer_HandAttackState::Create(CPlayer* pPlayer)
{
    return new CPlayer_HandAttackState(pPlayer);
}

void CPlayer_HandAttackState::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
    Safe_Release(m_pGameManager);
}
