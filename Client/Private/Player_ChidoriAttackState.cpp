#include "Player_ChidoriAttackState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"
#include "Player_RunState.h"
#include "Player_JumpState.h"
#include "Player_StepState.h"
#include "Player_SwordAttackState.h"

#pragma endregion

CPlayer_ChidoriAttackState::CPlayer_ChidoriAttackState(CPlayer* pPlayer)
    : m_pPlayer { pPlayer }
{
    Safe_AddRef(m_pPlayer);
}

void CPlayer_ChidoriAttackState::Start(_bool IsBlend)
{
    m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Aerial_Chidori_Run_Loop", 2.f, IsBlend);
}

CPlayerState* CPlayer_ChidoriAttackState::Update(_float fTimeDelta)
{
    CPlayerState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPlayer->Get_AnimProgress();

    //치도리 사용중 바라보는 방향으로 날아가기
    if (ANIM_STATE::ATTACK == m_eAnimState && m_fTimeAcc < 1.0f)
        m_pPlayer->Get_Transform()->Go_Direction(m_pPlayer->Get_Transform()->Get_State(STATE::LOOK), fTimeDelta * 2.f,
            m_pPlayer->Get_Navigation());

    else if (ANIM_STATE::ATTACK_END == m_eAnimState && fAnimProgress < 0.5f && false == IsAnimFinished)
        m_pPlayer->Get_Transform()->Go_Direction(m_pPlayer->Get_Transform()->Get_State(STATE::LOOK), fTimeDelta * 1.f * m_pGameInstance->Calc_Linear(-2.f, 1.f, fAnimProgress),
            m_pPlayer->Get_Navigation());


    if ((m_pGameInstance->Key_Pressing(DIK_A) ||
        m_pGameInstance->Key_Pressing(DIK_D))
        && ANIM_STATE::ATTACK_END != m_eAnimState)
    {
        if (m_pGameInstance->Key_Pressing(DIK_D))
            m_pPlayer->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 0.25f);

        if (m_pGameInstance->Key_Pressing(DIK_A))
            m_pPlayer->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -0.25f);
    }
    // 치도리 끝내는 동작 (ATTACK_END로 전환) 
    if (m_fTimeAcc >= 1.0f && ANIM_STATE::ATTACK == m_eAnimState)
    {
        m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Chidori_Attack_Lv3_End", 1.f, true);
        m_eAnimState = ANIM_STATE::ATTACK_END;
    }
    // 나선환 달리기 루프 시켜주기. 
    else if (true == IsAnimFinished && m_fTimeAcc < 1.0f && ANIM_STATE::ATTACK == m_eAnimState)
    {
        m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Aerial_Chidori_Run_Loop", 2.f, false, 0.f, true);
    }

    // IDLE 상태로 돌아가기. 
    if (true == IsAnimFinished && ANIM_STATE::ATTACK_END == m_eAnimState)
    {
        pNextState = CPlayer_IdleState::Create(m_pPlayer);
    }
    m_fTimeAcc += fTimeDelta;

    return pNextState;
}

_bool CPlayer_ChidoriAttackState::End()
{
    return true;
}

CPlayer_ChidoriAttackState* CPlayer_ChidoriAttackState::Create(CPlayer* pPlayer)
{
    return new CPlayer_ChidoriAttackState(pPlayer);
}

void CPlayer_ChidoriAttackState::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
}
