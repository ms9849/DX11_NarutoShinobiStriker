#include "Player_FireBallState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"

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
