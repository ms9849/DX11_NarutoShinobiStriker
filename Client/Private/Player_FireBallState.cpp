#include "Player_FireBallState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"
#include "Player_RunState.h"
#include "Player_StepState.h"
#include "Player_JumpState.h"

#pragma endregion

CPlayer_FireBallState::CPlayer_FireBallState(CPlayer* pPlayer)
    : m_pPlayer { pPlayer }
{
    Safe_AddRef(m_pPlayer);
}

void CPlayer_FireBallState::Start(_bool IsBlend)
{
    m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Fireball_Lv3", 1.75f, true);
}

CPlayerState* CPlayer_FireBallState::Update(_float fTimeDelta)
{
    CPlayerState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPlayer->Get_AnimProgress();

    if (true == IsAnimFinished)
        pNextState = CPlayer_IdleState::Create(m_pPlayer);

    else if ((m_pGameInstance->Key_Down(DIK_W) ||
        m_pGameInstance->Key_Down(DIK_A) ||
        m_pGameInstance->Key_Down(DIK_D))
        && fAnimProgress >= 0.8f)
        pNextState = CPlayer_RunState::Create(m_pPlayer);

    else if (m_pGameInstance->Key_Down(DIK_SPACE) && fAnimProgress >= 0.8f)
        pNextState = CPlayer_JumpState::Create(m_pPlayer);

    // 백스텝
    else if (m_pGameInstance->Key_Pressing(DIK_S) && fAnimProgress >= 0.8f)
    {
        // 돌다가 스텝 밟으면 상태 변경
        if (m_pGameInstance->Key_Down(DIK_LSHIFT))
            pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::BACK);
    }

    // 왼쪽 스텝
    else if (m_pGameInstance->Key_Pressing(DIK_A) && fAnimProgress >= 0.8f)
    {
        // 돌다가 스텝 밟으면 상태 변경
        if (m_pGameInstance->Key_Down(DIK_LSHIFT))
            pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::LEFT);
    }

    // 오른쪽 스텝
    else if (m_pGameInstance->Key_Pressing(DIK_D) && fAnimProgress >= 0.8f)
    {
        // 돌다가 스텝 밟으면 상태 변경
        if (m_pGameInstance->Key_Down(DIK_LSHIFT))
            pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::RIGHT);
    }

    return pNextState;
}

_bool CPlayer_FireBallState::End()
{
    return true;
}

CPlayer_FireBallState* CPlayer_FireBallState::Create(CPlayer* pPlayer)
{
    return new CPlayer_FireBallState(pPlayer);
}

void CPlayer_FireBallState::Free()
{
    __super::Free();
    Safe_Release(m_pPlayer);
}
