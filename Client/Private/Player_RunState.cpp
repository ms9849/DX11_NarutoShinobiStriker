#include "Player_RunState.h"

#include "Player.h"

#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"

#pragma endregion

CPlayer_RunState::CPlayer_RunState(CPlayer* pPlayer)
	: CPlayerState{}
	, m_pPlayer{ pPlayer }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_RunState::Start()
{
	m_pPlayer->Set_AnimIndex("CustomMan_Run_Loop");
	m_eAnimState = ANIM_STATE::RUN;
}

CPlayerState* CPlayer_RunState::Update(_float fTimeDelta)
{
	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_bool bBlend = { false };

	CPlayer_IdleState* pNextState = { nullptr };

	if (m_pGameInstance->Key_Pressing(DIK_W))
	{
		// 같은 애니 반복이면 보간 안하게. 
		m_pPlayer->Get_PlayerTransformPtr()->Go_Straight(fTimeDelta);

		// 멈추는 중이였다면 보간해서 달리는 애니 나오게 해주기.
		if (m_eAnimState == ANIM_STATE::RUN_END)
			bBlend = true;

		m_pPlayer->Set_AnimIndex("CustomMan_Run_Loop", 1.0f, bBlend);
		m_eAnimState = ANIM_STATE::RUN;
	}

	// Loop 재생중인데 키는 뗐다면, 
	else if (ANIM_STATE::RUN == m_eAnimState)
	{
		// End 애니메이션으로 전환.
		m_pPlayer->Set_AnimIndex("CustomMan_Run_End", 1.0f, false);
		m_eAnimState = ANIM_STATE::RUN_END;
	}

	else if (ANIM_STATE::RUN_END == m_eAnimState && true == IsAnimFinished)
		pNextState = CPlayer_IdleState::Create(m_pPlayer);

	return pNextState;
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
