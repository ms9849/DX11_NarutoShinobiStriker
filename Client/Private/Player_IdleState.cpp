#include "Player_IdleState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_RunState.h"

#pragma endregion

/* 서로 참조해서 안지워진다 ㅅㅂ.. */
CPlayer_IdleState::CPlayer_IdleState(CPlayer* pPlayer)
	: CPlayerState {}
	, m_pPlayer { pPlayer }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_IdleState::Start()
{
	m_pPlayer->Set_AnimIndex(0);
}

CPlayerState* CPlayer_IdleState::Update(_float fTimeDelta)
{
	CPlayerState* pNextState = { nullptr };

	if (m_pGameInstance->Key_Down(DIK_1))
		 pNextState = CPlayer_RunState::Create(m_pPlayer);

	return pNextState;
}

void CPlayer_IdleState::End()
{
}

CPlayer_IdleState* CPlayer_IdleState::Create(CPlayer* pPlayer)
{
	return new CPlayer_IdleState(pPlayer);
}

void CPlayer_IdleState::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
