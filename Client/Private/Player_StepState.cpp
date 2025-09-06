#include "Player_StepState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_RunState.h"
#include "Player_IdleState.h"
#include "Player_JumpState.h"
#include "Player_FrontJumpState.h"
#include "Player_HandAttackState.h"
#include "Player_SwordAttackState.h"
#include "Player_ChidoriReadyState.h"
#include "Player_FireBallState.h"
#include "Player_SuperSharkState.h"

#pragma endregion

CPlayer_StepState::CPlayer_StepState(CPlayer* pPlayer, ANIM_STATE eAnimState)
	: m_pPlayer { pPlayer }
	, m_eAnimState { eAnimState }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_StepState::Start(_bool IsBlend)
{
	if (ANIM_STATE::LEFT == m_eAnimState)
		m_pPlayer->Set_AnimIndex("CustomMan_DashStep_Left", 1.4f, IsBlend);

	else if (ANIM_STATE::RIGHT == m_eAnimState)
		m_pPlayer->Set_AnimIndex("CustomMan_DashStep_Right", 1.4f, IsBlend);

	else if (ANIM_STATE::FRONT == m_eAnimState)
		m_pPlayer->Set_AnimIndex("CustomMan_DashStep_Front", 1.4f, IsBlend);

	else if (ANIM_STATE::BACK == m_eAnimState)
		m_pPlayer->Set_AnimIndex("CustomMan_DashStep_Behind", 1.4f, IsBlend);
}

CPlayerState* CPlayer_StepState::Update(_float fTimeDelta)
{
	CPlayerState* pNextState = { nullptr };

	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pPlayer->Get_AnimProgress();

	if (true == IsAnimFinished)
		pNextState = CPlayer_IdleState::Create(m_pPlayer);

	//ratio 가져와서 일정 이상일떄만 이동
	// 왼쪽 짧대시
	else if (ANIM_STATE::LEFT == m_eAnimState)
	{
		_float fStepSpeed = m_pGameInstance->Calc_Quadratic(-5.f, 4.f, 1.f, fAnimProgress);

		/* 기본 공격.*/
		if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON) && fAnimProgress >= 0.5f)
		{
			if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
				pNextState = CPlayer_HandAttackState::Create(m_pPlayer);
			else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
				pNextState = CPlayer_SwordAttackState::Create(m_pPlayer);
		}

		/* 1번 스킬 사용 */
		else if (m_pGameInstance->Key_Down(DIK_1)
			&& nullptr == pNextState)
		{
			//스킬 사용 
			if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
				int a = 10;
			//pNextState = CPlayer_HandAttackState::Create(m_pPlayer);
			else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			{
				pNextState = CPlayer_ChidoriReadyState::Create(m_pPlayer);
			}
		}

		/* 2번 스킬 사용 */
		else if (m_pGameInstance->Key_Down(DIK_1)
			&& nullptr == pNextState)
		{
			//근접 타입
			if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
				int a = 10;
			//pNextState = CPlayer_HandAttackState::Create(m_pPlayer);
			//인술 타입 
			else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			{
				pNextState = CPlayer_FireBallState::Create(m_pPlayer);
			}
		}
		/* 3번 스킬 (필살기) 사용 */
		else if (m_pGameInstance->Key_Down(DIK_3)
			&& nullptr == pNextState)
		{
			// 근접 타입 
			if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
				int a = 10;
			//pNextState = CPlayer_HandAttackState::Create(m_pPlayer);
			// 인술 타입 
			else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			{
				pNextState = CPlayer_SuperSharkState::Create(m_pPlayer);
			}
		}

		else if ((m_pGameInstance->Key_Pressing(DIK_W) ||
		m_pGameInstance->Key_Pressing(DIK_A) ||
		m_pGameInstance->Key_Pressing(DIK_D)) 
		&& m_pGameInstance->Key_Down(DIK_SPACE)
		&& fAnimProgress >= 0.5f)
		pNextState = CPlayer_FrontJumpState::Create(m_pPlayer);

		else if (m_pGameInstance->Key_Down(DIK_SPACE) && fAnimProgress >= 0.5f)
			pNextState = CPlayer_JumpState::Create(m_pPlayer);

		else if ((m_pGameInstance->Key_Pressing(DIK_W) ||
				m_pGameInstance->Key_Pressing(DIK_A) ||
				m_pGameInstance->Key_Pressing(DIK_D))
				&& fAnimProgress >= 0.5f)
			pNextState = CPlayer_RunState::Create(m_pPlayer);

		m_pPlayer->Get_PlayerTransformPtr()->Go_Left(fTimeDelta * fStepSpeed);
	}
	// 오른쪽 짧대시
	else if (ANIM_STATE::RIGHT == m_eAnimState)
	{
		_float fStepSpeed = m_pGameInstance->Calc_Quadratic(-5.f, 4.f, 1.f, fAnimProgress);

		/* 기본 공격.*/
		if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON) && fAnimProgress >= 0.5f)
		{
			if(ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
				pNextState = CPlayer_HandAttackState::Create(m_pPlayer);
			else if(ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
				pNextState = CPlayer_SwordAttackState::Create(m_pPlayer);
		}

		/* 1번 스킬 사용 */
		else if (m_pGameInstance->Key_Down(DIK_1)
			&& nullptr == pNextState)
		{
			//스킬 사용 
			if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
				int a = 10;
			//pNextState = CPlayer_HandAttackState::Create(m_pPlayer);
			else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			{
				pNextState = CPlayer_ChidoriReadyState::Create(m_pPlayer);
			}
		}

		/* 2번 스킬 사용 */
		else if (m_pGameInstance->Key_Down(DIK_1)
			&& nullptr == pNextState)
		{
			//근접 타입
			if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
				int a = 10;
			//pNextState = CPlayer_HandAttackState::Create(m_pPlayer);
			//인술 타입 
			else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			{
				pNextState = CPlayer_FireBallState::Create(m_pPlayer);
			}
		}
		/* 3번 스킬 (필살기) 사용 */
		else if (m_pGameInstance->Key_Down(DIK_3)
			&& nullptr == pNextState)
		{
			// 근접 타입 
			if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
				int a = 10;
			//pNextState = CPlayer_HandAttackState::Create(m_pPlayer);
			// 인술 타입 
			else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			{
				pNextState = CPlayer_SuperSharkState::Create(m_pPlayer);
			}
		}

		else if ((m_pGameInstance->Key_Pressing(DIK_W) ||
			m_pGameInstance->Key_Pressing(DIK_A) ||
			m_pGameInstance->Key_Pressing(DIK_D))
			&& m_pGameInstance->Key_Down(DIK_SPACE)
			&& fAnimProgress >= 0.5f)
			pNextState = CPlayer_FrontJumpState::Create(m_pPlayer);

		else if (m_pGameInstance->Key_Down(DIK_SPACE) && fAnimProgress >= 0.5f)
			pNextState = CPlayer_JumpState::Create(m_pPlayer);

		else if ((m_pGameInstance->Key_Pressing(DIK_W) ||
				m_pGameInstance->Key_Pressing(DIK_A) ||
				m_pGameInstance->Key_Pressing(DIK_D))
				&& fAnimProgress >= 0.5f)
			pNextState = CPlayer_RunState::Create(m_pPlayer);

		m_pPlayer->Get_PlayerTransformPtr()->Go_Right(fTimeDelta * fStepSpeed);
	}
	// 백스텝 
	else if (ANIM_STATE::BACK == m_eAnimState)
	{
		_float fStepSpeed = m_pGameInstance->Calc_Quadratic(-7.86f, 3.57f, 1.f, fAnimProgress);

		/* 맨손 공격.*/
		if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON) && fAnimProgress >= 0.65f)
		{
			if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
				pNextState = CPlayer_HandAttackState::Create(m_pPlayer);

			else if(ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
				pNextState = CPlayer_SwordAttackState::Create(m_pPlayer);
		}

		/* 1번 스킬 사용 */
		else if (m_pGameInstance->Key_Down(DIK_1)
			&& nullptr == pNextState)
		{
			//스킬 사용 
			if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
				int a = 10;
			//pNextState = CPlayer_HandAttackState::Create(m_pPlayer);
			else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			{
				pNextState = CPlayer_ChidoriReadyState::Create(m_pPlayer);
			}
		}

		/* 2번 스킬 사용 */
		else if (m_pGameInstance->Key_Down(DIK_1)
			&& nullptr == pNextState)
		{
			//근접 타입
			if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
				int a = 10;
			//pNextState = CPlayer_HandAttackState::Create(m_pPlayer);
			//인술 타입 
			else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			{
				pNextState = CPlayer_FireBallState::Create(m_pPlayer);
			}
		}
		/* 3번 스킬 (필살기) 사용 */
		else if (m_pGameInstance->Key_Down(DIK_3)
			&& nullptr == pNextState)
		{
			// 근접 타입 
			if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
				int a = 10;
			//pNextState = CPlayer_HandAttackState::Create(m_pPlayer);
			// 인술 타입 
			else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			{
				pNextState = CPlayer_SuperSharkState::Create(m_pPlayer);
			}
		}

		else if (m_pGameInstance->Key_Down(DIK_SPACE) && fAnimProgress >= 0.65f)
			pNextState = CPlayer_JumpState::Create(m_pPlayer);

		else if ((m_pGameInstance->Key_Pressing(DIK_W) ||
			m_pGameInstance->Key_Pressing(DIK_A) ||
			m_pGameInstance->Key_Pressing(DIK_D))
			&& fAnimProgress >= 0.65f)
			pNextState = CPlayer_RunState::Create(m_pPlayer);

		else if (fAnimProgress != 0.f && fAnimProgress <= 0.6f)
			m_pPlayer->Get_PlayerTransformPtr()->Go_Backward(fTimeDelta * fStepSpeed);
	}
	// 앞 짧대시
	else if (ANIM_STATE::FRONT == m_eAnimState)
	{
		/* 맨손 공격.*/
		if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON) && fAnimProgress >= 0.35f)
		{
			if(ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
				pNextState = CPlayer_HandAttackState::Create(m_pPlayer);

			else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
				pNextState = CPlayer_SwordAttackState::Create(m_pPlayer);
		}

		/* 1번 스킬 사용 */
		else if (m_pGameInstance->Key_Down(DIK_1)
			&& nullptr == pNextState)
		{
			//스킬 사용 
			if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
				int a = 10;
			//pNextState = CPlayer_HandAttackState::Create(m_pPlayer);
			else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			{
				pNextState = CPlayer_ChidoriReadyState::Create(m_pPlayer);
			}
		}

		/* 2번 스킬 사용 */
		else if (m_pGameInstance->Key_Down(DIK_2)
			&& nullptr == pNextState)
		{
			//근접 타입
			if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
				int a = 10;
			//pNextState = CPlayer_HandAttackState::Create(m_pPlayer);
			//인술 타입 
			else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			{
				pNextState = CPlayer_FireBallState::Create(m_pPlayer);
			}
		}
		/* 3번 스킬 (필살기) 사용 */
		else if (m_pGameInstance->Key_Down(DIK_3)
			&& nullptr == pNextState)
		{
			// 근접 타입 
			if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
				int a = 10;
			//pNextState = CPlayer_HandAttackState::Create(m_pPlayer);
			// 인술 타입 
			else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			{
				pNextState = CPlayer_SuperSharkState::Create(m_pPlayer);
			}
		}

		else if (m_pGameInstance->Key_Down(DIK_SPACE) && fAnimProgress >= 0.35f)
			pNextState = CPlayer_JumpState::Create(m_pPlayer);

		else if ((m_pGameInstance->Key_Pressing(DIK_W) ||
			m_pGameInstance->Key_Pressing(DIK_A) ||
			m_pGameInstance->Key_Pressing(DIK_D))
			&& fAnimProgress >= 0.35f)
			pNextState = CPlayer_RunState::Create(m_pPlayer);

		else if (fAnimProgress >= 0.35f && m_pGameInstance->Key_Pressing(DIK_S))
		{
			// 돌다가 스텝 밟으면 상태 변경
			if (m_pGameInstance->Key_Down(DIK_LSHIFT))
				pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::BACK);
		}


		if (fAnimProgress < 0.45f)
		{
			_float fStepSpeed = m_pGameInstance->Calc_Quadratic(-29.3f, 10.5f, 1.2f, fAnimProgress);
			m_pPlayer->Get_PlayerTransformPtr()->Go_Straight(fTimeDelta * fStepSpeed);
		}
		
	}

	return pNextState;
}

_bool CPlayer_StepState::End()
{
	return true;
}

CPlayer_StepState* CPlayer_StepState::Create(CPlayer* pPlayer, ANIM_STATE eAnimState)
{
	return new CPlayer_StepState(pPlayer, eAnimState);
}

void CPlayer_StepState::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
