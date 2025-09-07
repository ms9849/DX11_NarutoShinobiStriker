#include "Player_RunState.h"

#include "Player.h"

#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"
#include "Player_JumpState.h"
#include "Player_FrontJumpState.h"
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

CPlayer_RunState::CPlayer_RunState(CPlayer* pPlayer)
	: CPlayerState{}
	, m_pPlayer{ pPlayer }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_RunState::Start(_bool IsBlend)
{
 	m_pPlayer->Set_AnimIndex("CustomMan_Run_Loop", 1.0f, IsBlend);
	m_eAnimState = ANIM_STATE::RUN;
}

CPlayerState* CPlayer_RunState::Update(_float fTimeDelta)
{
	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_bool IsBlend = { false };
	_bool IsRunning = { false };
	CPlayerState* pNextState = { nullptr };

	//앞 이동
	if (m_pGameInstance->Key_Pressing(DIK_W) || 
		m_pGameInstance->Key_Pressing(DIK_A) ||
		m_pGameInstance->Key_Pressing(DIK_D)
		)
	{
		// 같은 애니 반복이면 보간 안하게. 
		m_pPlayer->Get_PlayerTransformPtr()->Go_Straight(fTimeDelta);

		// 멈추는 중이였다면 보간해서 달리는 애니 나오게 해주기.
		if (m_eAnimState == ANIM_STATE::RUN_END)
			IsBlend = true;
		else
			IsBlend = false;
		
		m_pPlayer->Set_AnimIndex("CustomMan_Run_Loop", 1.0f, IsBlend);
		m_eAnimState = ANIM_STATE::RUN;

		IsRunning = true;

		// 짧은 앞대시
		if (m_pGameInstance->Key_Pressing(DIK_W))
		{
			// 돌다가 스텝 밟으면 상태 변경
			if (m_pGameInstance->Key_Down(DIK_LSHIFT) && nullptr == pNextState)
			{
				pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::FRONT);
			}
		}

		// 오른쪽으로 돌아 
		if (m_pGameInstance->Key_Pressing(DIK_D))
		{
			m_pPlayer->Get_PlayerTransformPtr()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
			
			// 돌다가 스텝 밟으면 상태 변경
			if (m_pGameInstance->Key_Down(DIK_LSHIFT) && nullptr == pNextState)
			{
				pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::RIGHT);
			}
		}
		// 왼쪽으로 돌아
		if (m_pGameInstance->Key_Pressing(DIK_A))
		{
			m_pPlayer->Get_PlayerTransformPtr()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
			
			// 돌다가 스텝 밟으면 상태 변경
			if (m_pGameInstance->Key_Down(DIK_LSHIFT) && nullptr == pNextState)
			{
				pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::LEFT);
			}
		}
		
		if (m_pGameInstance->Key_Down(DIK_SPACE))
		{
			pNextState = CPlayer_FrontJumpState::Create(m_pPlayer);
		}
	}
	
	/* 맨손 공격.*/
	if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON)
		&& nullptr == pNextState)
	{
		if(ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_HandAttackState::Create(m_pPlayer);
		else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_SwordAttackState::Create(m_pPlayer);

	}
	/* 1번 스킬 사용 */
	else if (m_pGameInstance->Key_Down(DIK_1))
	{
		// 근접 타입 
		if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_RasenganReadyState::Create(m_pPlayer);

		else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_ChidoriReadyState::Create(m_pPlayer);
	}
	/* 2번 스킬 사용 */
	else if (m_pGameInstance->Key_Down(DIK_2))
	{
		// 근접 타입 
		if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_RasenShurikenState::Create(m_pPlayer);

		else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_FireBallState::Create(m_pPlayer);
	}
	/* 3번 스킬 (필살기) 사용 */
	else if (m_pGameInstance->Key_Down(DIK_3))
	{
		// 근접 타입 
		if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_KamuiState::Create(m_pPlayer);

		else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_SuperSharkState::Create(m_pPlayer);
	}


	// 백스텝
	if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		// 돌다가 스텝 밟으면 상태 변경
		if (m_pGameInstance->Key_Down(DIK_LSHIFT) && nullptr == pNextState)
		{
			pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::BACK);
		}
	}

	// Loop 재생중인데 키는 뗐다면, 
	if (ANIM_STATE::RUN == m_eAnimState && false == IsRunning)
	{
		// End 애니메이션으로 전환.
		m_pPlayer->Set_AnimIndex("CustomMan_Run_End", 1.0f, false);
		m_eAnimState = ANIM_STATE::RUN_END;
	}
	else if (ANIM_STATE::RUN_END == m_eAnimState && 
		m_pGameInstance->Key_Down(DIK_SPACE) && false == IsRunning && nullptr == pNextState)
		pNextState = CPlayer_JumpState::Create(m_pPlayer);
	 
	else if (ANIM_STATE::RUN_END == m_eAnimState && true == IsAnimFinished && nullptr == pNextState)
	{
		pNextState = CPlayer_IdleState::Create(m_pPlayer);
		m_IsNextAnimBlened = false;
	}

	return pNextState;
}

_bool CPlayer_RunState::End()
{
	return m_IsNextAnimBlened;
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
