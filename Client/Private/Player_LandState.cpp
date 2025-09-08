#include "Player_LandState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"
#include "Player_JumpState.h"
#include "Player_RunState.h"
#include "Player_StepState.h"
#include "Player_HandAttackState.h"
#include "Player_SwordAttackState.h"

#include "Player_ChidoriReadyState.h"
#include "Player_FireBallState.h"
#include "Player_SuperSharkState.h"

#include "Player_RasenganReadyState.h"
#include "Player_RasenShurikenState.h"
#include "Player_KamuiState.h"

#pragma endregion

CPlayer_LandState::CPlayer_LandState(CPlayer* pPlayer)
	: m_pPlayer { pPlayer }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_LandState::Start(_bool IsBlend)
{
	m_pPlayer->Set_AnimIndex("CustomMan_Land", 1.f, IsBlend, 0.4f);
}

CPlayerState* CPlayer_LandState::Update(_float fTimeDelta)
{
	CPlayerState* pNextState = { nullptr };

	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pPlayer->Get_AnimProgress();

	if ((m_pGameInstance->Key_Pressing(DIK_W) ||m_pGameInstance->Key_Pressing(DIK_A) || m_pGameInstance->Key_Pressing(DIK_D))
		&& fAnimProgress >= 0.15f)
	{
		pNextState = CPlayer_RunState::Create(m_pPlayer);
		m_IsNextAnimBlened = true;
	}

	/* 맨손 공격.*/
	else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON))
	{
		if(ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
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
			pNextState = CPlayer_SuperSharkState::Create(m_pPlayer);
	}

	//점프
	else if (m_pGameInstance->Key_Down(DIK_SPACE)
		&& fAnimProgress >= 0.15f)
	{
		pNextState = CPlayer_JumpState::Create(m_pPlayer);
		m_IsNextAnimBlened = true;
	}

	// 백스텝
	else if (m_pGameInstance->Key_Pressing(DIK_S) && m_pGameInstance->Key_Down(DIK_LSHIFT) 
		&& fAnimProgress >= 0.2f)
	{
		pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::BACK);
		m_IsNextAnimBlened = true;
	}

	// 오른쪽 스텝
	else if (m_pGameInstance->Key_Pressing(DIK_D))
	{
		if (m_pGameInstance->Key_Down(DIK_LSHIFT) && nullptr == pNextState)
		{
			pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::RIGHT);
		}
	}
	// 왼쪽 스텝
	else if (m_pGameInstance->Key_Pressing(DIK_A))
	{
		if (m_pGameInstance->Key_Down(DIK_LSHIFT) && nullptr == pNextState)
		{
			pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::LEFT);
		}
	}
	//애니 끝나면 돌아가. 
	else if (true == IsAnimFinished)
	{
		pNextState = CPlayer_IdleState::Create(m_pPlayer);
		m_IsNextAnimBlened = false;
	}

	return pNextState;
}

_bool CPlayer_LandState::End()
{
	return m_IsNextAnimBlened;
}

CPlayer_LandState* CPlayer_LandState::Create(CPlayer* pPlayer)
{
	return new CPlayer_LandState(pPlayer);
}

void CPlayer_LandState::Free()
{
	__super::Free();
	Safe_Release(m_pPlayer);
}
