#include "Player_RasenShurikenState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"

#pragma endregion

/*
지상 나선 수리검.
크게 신경쓸 예외 사항은 없음
*/

CPlayer_RasenShurikenState::CPlayer_RasenShurikenState(CPlayer* pPlayer)
    : m_pPlayer { pPlayer }
{
    Safe_AddRef(m_pPlayer);
}

void CPlayer_RasenShurikenState::Start(_bool IsBlend)
{
    m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_TrueRasenShuriken", 1.2f);
}

CPlayerState* CPlayer_RasenShurikenState::Update(_float fTimeDelta)
{
    CPlayerState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);

    if (true == IsAnimFinished)
        pNextState = CPlayer_IdleState::Create(m_pPlayer);

    return pNextState;
}

_bool CPlayer_RasenShurikenState::End()
{
    return true;
}

CPlayer_RasenShurikenState* CPlayer_RasenShurikenState::Create(CPlayer* pPlayer)
{
    return new CPlayer_RasenShurikenState(pPlayer);
}

void CPlayer_RasenShurikenState::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
}
