#include "Player_BeatenState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"
#include "Player_RunState.h"
#include "Player_StepState.h"
#include "Player_JumpState.h"

#pragma endregion

CPlayer_BeatenState::CPlayer_BeatenState(CPlayer* pPlayer)
	: m_pPlayer { pPlayer }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_BeatenState::Start(_bool IsBlend)
{
}

CPlayerState* CPlayer_BeatenState::Update(_float fTimeDelta)
{
	CPlayerState* pNextState = { nullptr };

	return pNextState;
}

_bool CPlayer_BeatenState::End()
{
	return true;
}

CPlayer_BeatenState* CPlayer_BeatenState::Create(CPlayer* pPlayer)
{
	return new CPlayer_BeatenState(pPlayer);
}

void CPlayer_BeatenState::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
