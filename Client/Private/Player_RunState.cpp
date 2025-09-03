#include "Player_RunState.h"

#include "Player.h"

CPlayer_RunState::CPlayer_RunState(CPlayer* pPlayer)
	: CPlayerState{}
	, m_pPlayer{ pPlayer }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_RunState::Start()
{
	m_pPlayer->Set_AnimIndex(1);
}

CPlayerState* CPlayer_RunState::Update(_float fTimeDelta)
{
	return nullptr;
}

void CPlayer_RunState::End()
{
}

CPlayer_RunState* CPlayer_RunState::Create(CPlayer* pPlayer)
{
	return new CPlayer_RunState(pPlayer);
}

void CPlayer_RunState::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
