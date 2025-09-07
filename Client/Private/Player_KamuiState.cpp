#include "Player_KamuiState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"

#pragma endregion

CPlayer_KamuiState::CPlayer_KamuiState(CPlayer* pPlayer)
    : m_pPlayer { pPlayer }
{
    Safe_AddRef(m_pPlayer);
}

void CPlayer_KamuiState::Start(_bool IsBlend)
{
    m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_KamuiKakashi", 1.5f, true);
}

CPlayerState* CPlayer_KamuiState::Update(_float fTimeDelta)
{
    CPlayerState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);

    if (true == IsAnimFinished)
        pNextState = CPlayer_IdleState::Create(m_pPlayer);

    return pNextState;
}

_bool CPlayer_KamuiState::End()
{
    return true;
}

CPlayer_KamuiState* CPlayer_KamuiState::Create(CPlayer* pPlayer)
{
    return new CPlayer_KamuiState(pPlayer);
}

void CPlayer_KamuiState::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
}
