#include "Player_ChidoriAttackState.h"

#include "Player.h"
#include "GameInstance.h"
#include "GameManager.h"

#include "Chidori.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"
#include "Player_RunState.h"
#include "Player_JumpState.h"
#include "Player_StepState.h"
#include "Player_SwordAttackState.h"

#pragma endregion

CPlayer_ChidoriAttackState::CPlayer_ChidoriAttackState(CPlayer* pPlayer, CChidori* pChidori)
    : m_pPlayer { pPlayer }
    , m_pGameManager { CGameManager::GetInstance() }
    , m_pChidori { pChidori }
{
    Safe_AddRef(m_pPlayer);
    Safe_AddRef(m_pGameManager);
    Safe_AddRef(m_pChidori);
}

void CPlayer_ChidoriAttackState::Start(_bool IsBlend)
{
    m_pPlayer->Set_Invincible(true);
    m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Aerial_Chidori_Run_Loop", 2.f, IsBlend);

    m_pGameInstance->PlaySoundOnce(TEXT("Chidori_Voice.wav"), CHANNELID::EFFECT, 0.7f);
    m_eAnimState = ANIM_STATE::ATTACK;
    m_fTimeAcc = 0.f;
}

CPlayerState* CPlayer_ChidoriAttackState::Update(_float fTimeDelta)
{
    CPlayerState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPlayer->Get_AnimProgress();

    //치도리 사용중 바라보는 방향으로 날아가기
    if (ANIM_STATE::ATTACK == m_eAnimState && m_fTimeAcc < 1.0f)
        m_pPlayer->Get_Transform()->Go_Direction(m_pPlayer->Get_Transform()->Get_State(STATE::LOOK), fTimeDelta * 2.f,
            nullptr);

    else if (ANIM_STATE::ATTACK_END == m_eAnimState && fAnimProgress < 0.5f && false == IsAnimFinished)
        m_pPlayer->Get_Transform()->Go_Direction(m_pPlayer->Get_Transform()->Get_State(STATE::LOOK), fTimeDelta * 1.f * m_pGameInstance->Calc_Linear(-2.f, 1.f, fAnimProgress),
            nullptr);

    // 추적
    CTransform* pTargetTransform = m_pGameManager->Calc_Target(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));

    if (nullptr != pTargetTransform && ANIM_STATE::ATTACK_END != m_eAnimState)
    {
        _vector vTargetPosition = pTargetTransform->Get_State(STATE::POSITION);
        m_pPlayer->Get_Transform()->LookAt_Lerp(vTargetPosition);
    }
    else if ((m_pGameInstance->Key_Pressing(DIK_A) ||
        m_pGameInstance->Key_Pressing(DIK_D))
        && ANIM_STATE::ATTACK_END != m_eAnimState)
    {
        if (m_pGameInstance->Key_Pressing(DIK_D))
            m_pPlayer->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 0.5f);

        if (m_pGameInstance->Key_Pressing(DIK_A))
            m_pPlayer->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -0.5f);
    }

    if (0.8f <= fAnimProgress && ANIM_STATE::ATTACK_END == m_eAnimState)
    {
        m_pChidori->Set_Visible(false);
    }

    // 치도리 끝내는 동작 (ATTACK_END로 전환) 
    if (ANIM_STATE::ATTACK == m_eAnimState && (false == m_pChidori->IsColliderActive() || m_fTimeAcc >= 1.0f))
    {
        m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Chidori_Attack_Lv3_End", 1.f, true);
        m_eAnimState = ANIM_STATE::ATTACK_END;
    }
    // 치도리 달리기 루프 시켜주기. 
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
    m_pPlayer->Set_Invincible(false);
    m_pChidori->Set_Dead(true); 

    return true;
}

CPlayer_ChidoriAttackState* CPlayer_ChidoriAttackState::Create(CPlayer* pPlayer, CChidori* pChidori)
{
    return new CPlayer_ChidoriAttackState(pPlayer, pChidori);
}

void CPlayer_ChidoriAttackState::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
    Safe_Release(m_pChidori);
    Safe_Release(m_pGameManager);
}
