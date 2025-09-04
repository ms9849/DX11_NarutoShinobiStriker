#include "Player_RunState.h"

#include "Player.h"

#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"
#include "Player_JumpState.h"
#include "Player_FrontJumpState.h"
#include "Player_StepState.h"

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
			if (m_pGameInstance->Key_Down(DIK_LSHIFT))
				pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::FRONT);
		}

		// 오른쪽으로 돌아 
		if (m_pGameInstance->Key_Pressing(DIK_D))
		{
			m_pPlayer->Get_PlayerTransformPtr()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
			
			// 돌다가 스텝 밟으면 상태 변경
			if (m_pGameInstance->Key_Down(DIK_LSHIFT))
				pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::RIGHT);
		}
		// 왼쪽으로 돌아
		if (m_pGameInstance->Key_Pressing(DIK_A) )
		{
			m_pPlayer->Get_PlayerTransformPtr()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
			
			// 돌다가 스텝 밟으면 상태 변경
			if (m_pGameInstance->Key_Down(DIK_LSHIFT))
				pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::LEFT);
		}
		
		if (m_pGameInstance->Key_Down(DIK_SPACE))
			pNextState = CPlayer_FrontJumpState::Create(m_pPlayer);
	}

	// Loop 재생중인데 키는 뗐다면, 
	if (ANIM_STATE::RUN == m_eAnimState && false == IsRunning)
	{
		// End 애니메이션으로 전환.
		m_pPlayer->Set_AnimIndex("CustomMan_Run_End", 1.0f, false);
		m_eAnimState = ANIM_STATE::RUN_END;
	}
	else if (ANIM_STATE::RUN_END == m_eAnimState && 
		m_pGameInstance->Key_Down(DIK_SPACE) && false == IsRunning)
		pNextState = CPlayer_JumpState::Create(m_pPlayer);
	 
	else if (ANIM_STATE::RUN_END == m_eAnimState && true == IsAnimFinished)
		pNextState = CPlayer_IdleState::Create(m_pPlayer);

	// 백스텝
	if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		// 돌다가 스텝 밟으면 상태 변경
		if (m_pGameInstance->Key_Down(DIK_LSHIFT))
			pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::BACK);
	}

	return pNextState;
}

_bool CPlayer_RunState::End()
{
	return true;
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
