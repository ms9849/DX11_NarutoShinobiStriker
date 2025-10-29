#include "Player_SwordAttackState.h"

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

CPlayer_SwordAttackState::CPlayer_SwordAttackState(CPlayer* pPlayer)
    : m_pPlayer { pPlayer }
    , m_pGameManager { CGameManager::GetInstance() }
{
    Safe_AddRef(m_pPlayer);
    Safe_AddRef(m_pGameManager);
}

void CPlayer_SwordAttackState::Start(_bool IsBlend)
{
    CTransform* pTargetTransform = m_pGameManager->Calc_Target(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));

    if (nullptr != pTargetTransform)
        m_pPlayer->Get_Transform()->LookAt_XZ(pTargetTransform->Get_State(STATE::POSITION));

    m_IsOnCollider = false;
    m_pPlayer->Set_AnimIndex("CustomMan_Attack_SnakeSword_cmb_01", 2.f, true );
    m_eAnimState = ANIM_STATE::ATTACK_01;

    m_pGameInstance->PlaySoundOnce(TEXT("SwordSwing_Voice1.wav"), CHANNELID::EFFECT3, 0.5f);
    m_pGameInstance->PlaySoundOnce(TEXT("SwordSwing1.wav"), CHANNELID::EFFECT3, 0.2f);
}

CPlayerState* CPlayer_SwordAttackState::Update(_float fTimeDelta)
{
    CTransform* pTargetTransform = m_pGameManager->Calc_Target(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));
    CPlayerState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPlayer->Get_AnimProgress();
    Update_Collider(fAnimProgress);

    /* CHASE */
    if (nullptr != pTargetTransform && fAnimProgress <= 0.5f)
    {
        _float fTargetDist = XMVectorGetX(XMVector3Length(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION) - pTargetTransform->Get_State(STATE::POSITION)));
        if (fTargetDist <= 3.f && ANIM_STATE::ATTACK_03 != m_eAnimState)
            m_pPlayer->Get_Transform()->Chase_XZ(pTargetTransform->Get_State(STATE::POSITION), fTimeDelta * 0.1f, nullptr, 1.2f);

    }
    /* chase 끝 */

    if (false == IsAnimFinished && fAnimProgress <= 0.5f && ANIM_STATE::ATTACK_01 == m_eAnimState)
    {
        m_pPlayer->Get_Transform()->Go_Straight(0.5f * fTimeDelta * m_pGameInstance->Calc_Linear(-1.4f, 0.7f, fAnimProgress),
            nullptr);
    }

    else  if (false == IsAnimFinished && fAnimProgress <= 0.3f && ANIM_STATE::ATTACK_02 == m_eAnimState)
    {
        m_pPlayer->Get_Transform()->Go_Straight(0.5f * fTimeDelta * m_pGameInstance->Calc_Linear(-2.3f, 0.7f, fAnimProgress),
            nullptr);
    }

    else if (false == IsAnimFinished && fAnimProgress <= 0.35f && fAnimProgress >= 0.25f && ANIM_STATE::ATTACK_03 == m_eAnimState)
    {
        m_pPlayer->Get_Transform()->Go_Straight(fTimeDelta * m_pGameInstance->Calc_Linear(-4.f, 1.4f, fAnimProgress),
            nullptr);
    }


    if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON)
        && m_eAnimState == ANIM_STATE::ATTACK_01 && fAnimProgress >= 0.5f)
    {
        if (nullptr != pTargetTransform)
            m_pPlayer->Get_Transform()->LookAt_XZ(pTargetTransform->Get_State(STATE::POSITION));

        m_IsOnCollider = false;
        m_pPlayer->Set_AnimIndex("CustomMan_Attack_SnakeSword_cmb_02", 2.f, true);
        m_eAnimState = ANIM_STATE::ATTACK_02;
        m_pGameInstance->PlaySoundOnce(TEXT("SwordSwing_Voice2.wav"), CHANNELID::EFFECT3, 0.6f);
        m_pGameInstance->PlaySoundOnce(TEXT("SwordSwing2.wav"), CHANNELID::EFFECT3, 0.2f);
    }

    else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON)
        && m_eAnimState == ANIM_STATE::ATTACK_02 && fAnimProgress >= 0.5f)
    {
        if (nullptr != pTargetTransform)
            m_pPlayer->Get_Transform()->LookAt_XZ(pTargetTransform->Get_State(STATE::POSITION));

        m_IsOnCollider = false;
        m_pPlayer->Set_AnimIndex("CustomMan_Attack_SnakeSword_cmb_03", 3.f, true);
        m_eAnimState = ANIM_STATE::ATTACK_03;
        m_pGameInstance->PlaySoundOnce(TEXT("SwordSwing_Voice3.wav"), CHANNELID::EFFECT3, 0.6f);
        m_pGameInstance->PlaySoundOnce(TEXT("SwordSwing1.wav"), CHANNELID::EFFECT3, 0.2f);
    }


    // IDLE
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
            pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::RIGHT);
    }
    // 왼쪽 스텝
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
    m_pPlayer->Set_WeaponCollider_Active(false);

    return true;
}

void CPlayer_SwordAttackState::Update_Collider(_float fAnimProgress)
{
    /* 콜라이더 온/오프 */
    if (ANIM_STATE::ATTACK_03 != m_eAnimState
        && (fAnimProgress >= 0.8f || fAnimProgress <= 0.05f))
        m_pPlayer->Set_WeaponCollider_Active(false);

    /* 마지막 공격이라면 다른 조건 줘서 콜라이더 끄기 */
    else if (ANIM_STATE::ATTACK_03 == m_eAnimState
        && (fAnimProgress >= 0.6f || fAnimProgress <= 0.25f))
        m_pPlayer->Set_WeaponCollider_Active(false);

    /* 만약 콜라이더가 켜질수 있는 상태라면 */
    else if (m_IsOnCollider == false)
    {
        /* 콜라이더 켜주기 */
        m_pPlayer->Set_WeaponCollider_Active(true);
        m_IsOnCollider = true;
    }

    /* 콜라이더 세팅 */

    if (ANIM_STATE::ATTACK_03 == m_eAnimState)
    {
        CGameManager::GetInstance()->Add_Collider_ToCollision(TEXT("Player_Attack"), COLLIDER_HANDLE_ID::PLAYER_SWORD_ATTACK_FINAL,
            m_pPlayer->Get_WeaponCollider());
    }
    else if(ANIM_STATE::ATTACK_03 != m_eAnimState)
    {
        CGameManager::GetInstance()->Add_Collider_ToCollision(TEXT("Player_Attack"), COLLIDER_HANDLE_ID::PLAYER_SWORD_ATTACK,
            m_pPlayer->Get_WeaponCollider());
    }
}

CPlayer_SwordAttackState* CPlayer_SwordAttackState::Create(CPlayer* pPlayer)
{
    return new CPlayer_SwordAttackState(pPlayer);
}

void CPlayer_SwordAttackState::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
    Safe_Release(m_pGameManager);
}
