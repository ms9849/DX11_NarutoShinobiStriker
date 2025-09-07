#include "Player_RasenganReadyState.h"

#include "Player.h"
#include "GameInstance.h"


/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

// 나선환 실제 공격으로 넘어가기
#include "Player_RasenganState.h"

#pragma endregion


CPlayer_RasenganReadyState::CPlayer_RasenganReadyState(CPlayer* pPlayer)
    : m_pPlayer { pPlayer }
{
    Safe_AddRef(m_pPlayer);
}

void CPlayer_RasenganReadyState::Start(_bool IsBlend)
{
    m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Rasengun_Charge_Lv2toLv3", 1.f, true);
}

CPlayerState* CPlayer_RasenganReadyState::Update(_float fTimeDelta)
{
    CPlayerState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);

    if (true == IsAnimFinished)
    {
        pNextState = CPlayer_RasenganState::Create(m_pPlayer);
    }

    return pNextState;
}

_bool CPlayer_RasenganReadyState::End()
{
    return true;
}

CPlayer_RasenganReadyState* CPlayer_RasenganReadyState::Create(CPlayer* pPlayer)
{
    return new CPlayer_RasenganReadyState(pPlayer);
}

void CPlayer_RasenganReadyState::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
}
