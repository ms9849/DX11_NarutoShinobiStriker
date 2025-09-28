#include "Player_IdleState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_RunState.h"
#include "Player_JumpState.h"
#include "Player_FrontJumpState.h"
#include "Player_StepState.h"
#include "Player_DuckingState.h"

/* 기본 공격 */
#include "Player_HandAttackState.h"
#include "Player_SwordAttackState.h"

/* 밀리 타입 */
#include "Player_ChidoriReadyState.h"
#include "Player_FireBallState.h"
#include "Player_BigSharkState.h"

/* 인술 타입 */
#include "Player_RasenganReadyState.h"
#include "Player_RasenShurikenState.h"
#include "Player_KamuiState.h"

#pragma endregion

/* 서로 참조해서 안지워진다 ㅅㅂ.. */
CPlayer_IdleState::CPlayer_IdleState(CPlayer* pPlayer)
	: m_pPlayer { pPlayer }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_IdleState::Start(_bool IsBlend)
{
 	m_pPlayer->Set_AnimIndex("CustomMan_Idle_Loop", 1.f, IsBlend, 0.1f);
}

CPlayerState* CPlayer_IdleState::Update(_float fTimeDelta)
{
	m_pPlayer->Play_Animation(fTimeDelta);

	CPlayerState* pNextState = { nullptr };

	if (m_pGameInstance->Key_Pressing(DIK_LCONTROL))
		pNextState = CPlayer_DuckingState::Create(m_pPlayer);

	/* 맨손 공격.*/
	else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON))
	{
		if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_HandAttackState::Create(m_pPlayer);

		else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_SwordAttackState::Create(m_pPlayer);
	}
	/* 1번 스킬 사용 */
	else if (m_pGameInstance->Key_Down(DIK_1) && m_pPlayer->Use_Skill(SKILLNUM::SECOND))
	{ 
		// 근접 타입 
		if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_RasenganReadyState::Create(m_pPlayer);

		else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_ChidoriReadyState::Create(m_pPlayer);
	}
	/* 2번 스킬 사용 */
	else if (m_pGameInstance->Key_Down(DIK_2) && m_pPlayer->Use_Skill(SKILLNUM::THIRD))
	{
		// 근접 타입 
		if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_RasenShurikenState::Create(m_pPlayer);

		else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_FireBallState::Create(m_pPlayer);
	}
	/* 3번 스킬 (필살기) 사용 */
	else if (m_pGameInstance->Key_Down(DIK_3) && m_pPlayer->Use_Skill(SKILLNUM::SPECIAL))
	{
		// 근접 타입 
		if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_KamuiState::Create(m_pPlayer);

		else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_BigSharkState::Create(m_pPlayer);
	}

	else if (m_pGameInstance->Key_Down(DIK_W) || 
		m_pGameInstance->Key_Down(DIK_A) ||
		m_pGameInstance->Key_Down(DIK_D)
		)
		 pNextState = CPlayer_RunState::Create(m_pPlayer);

	else if (m_pGameInstance->Key_Down(DIK_SPACE))
		 pNextState = CPlayer_JumpState::Create(m_pPlayer);

	// 백스텝
	else if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		// 돌다가 스텝 밟으면 상태 변경
		if (m_pGameInstance->Key_Down(DIK_LSHIFT))
			pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::BACK);
	}

	return pNextState;
}

_bool CPlayer_IdleState::End()
{
	return true;
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
