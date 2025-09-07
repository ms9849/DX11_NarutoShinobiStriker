#include "Player_DuckingState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_SuperJumpState.h"
#include "Player_IdleState.h"

#pragma endregion

CPlayer_DuckingState::CPlayer_DuckingState(CPlayer* pPlayer)
	: m_pPlayer { pPlayer }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_DuckingState::Start(_bool IsBlend)
{
	m_pPlayer->Set_AnimIndex("CustomMan_ChakraJump_Charge_Start", 1.f, IsBlend);
	m_eAnimState = ANIM_STATE::START;
}

CPlayerState* CPlayer_DuckingState::Update(_float fTimeDelta)
{
	CPlayerState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);

	if (m_pGameInstance->Key_Pressing(DIK_LCONTROL))
	{
		m_fTimeAcc += fTimeDelta;

		if (m_fTimeAcc > 2.f)
			m_fTimeAcc = 2.f;
	}
	else if(m_fTimeAcc < 2.f)
	{
		pNextState = CPlayer_IdleState::Create(m_pPlayer);
	}
	else if(m_fTimeAcc >= 2.f)
	{
		pNextState = CPlayer_SuperJumpState::Create(m_pPlayer, m_fTimeAcc);
	}
	/* 시작 애니메이션에서 루프로 */
	if (true == IsAnimFinished && ANIM_STATE::START == m_eAnimState)
	{
		m_pPlayer->Set_AnimIndex("CustomMan_ChakraJump_Charge_Loop", 1.f, false);
		m_eAnimState = ANIM_STATE::LOOP;
	}
	/* 루프 무한 반복 */
	else if (true == IsAnimFinished && ANIM_STATE::LOOP == m_eAnimState)
		m_pPlayer->Set_AnimIndex("CustomMan_ChakraJump_Charge_Loop", 1.f, false, 0.f, true);

	return pNextState;
}

_bool CPlayer_DuckingState::End()
{
	return true;
}

CPlayer_DuckingState* CPlayer_DuckingState::Create(CPlayer* pPlayer)
{
	return new CPlayer_DuckingState(pPlayer);
}

void CPlayer_DuckingState::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
