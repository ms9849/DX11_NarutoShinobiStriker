#include "Player_SuperSharkState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"

#pragma endregion


CPlayer_SuperSharkState::CPlayer_SuperSharkState(CPlayer* pPlayer)
	: m_pPlayer { pPlayer }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_SuperSharkState::Start(_bool IsBlend)
{
	m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_SuperSharkBomb", 1.5f, IsBlend, 0.1f);
}

CPlayerState* CPlayer_SuperSharkState::Update(_float fTimeDelta)
{
	CPlayerState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);

	if (true == IsAnimFinished)
		pNextState = CPlayer_IdleState::Create(m_pPlayer);

	return pNextState;
}

_bool CPlayer_SuperSharkState::End()
{
	return true;
}

CPlayer_SuperSharkState* CPlayer_SuperSharkState::Create(CPlayer* pPlayer)
{
	return new CPlayer_SuperSharkState(pPlayer);
}

void CPlayer_SuperSharkState::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
