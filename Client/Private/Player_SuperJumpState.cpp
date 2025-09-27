#include "Player_SuperJumpState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_LandState.h"
#include "Player_IdleState.h"

#pragma endregion

CPlayer_SuperJumpState::CPlayer_SuperJumpState(CPlayer* pPlayer, _float fPower)
	: m_pPlayer { pPlayer }
	, m_fPower { fPower }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_SuperJumpState::Start(_bool IsBlend)
{
	m_pPlayer->Set_Ground(false);
	m_pPlayer->Set_Pickable(false);

	m_pPlayer->Set_AnimIndex("CustomMan_ChakraJump_Charge_End", 0.7f, false);
	m_eAnimState = ANIM_STATE::START;
}

CPlayerState* CPlayer_SuperJumpState::Update(_float fTimeDelta)
{
	if (m_fTimeAcc >= 0.8f)
		m_pPlayer->Set_Pickable(true);
	
	CPlayerState* pNextState = { nullptr };
	_bool IsAnimFinished; 
	_float fAnimProgress = m_pPlayer->Get_AnimProgress();

	_float fStartSpeed = 10.f;    // 초기 점프 속도
	_float fGravity = 7.f;    // 중력
	_float fJumpScale = 0.16f; // 전체 스케일 (최대 높이)

	m_fTimeAcc += fTimeDelta * 2.1f;

	m_fMovement = fStartSpeed * m_fTimeAcc - 0.5f * fGravity * m_fTimeAcc * m_fTimeAcc;
	m_fMovement *= fJumpScale;

	if (m_fMovement < -0.3f)
		m_fMovement = -0.3f;

	/* 시작 한번만 재생. */
	if (ANIM_STATE::START == m_eAnimState)
	{
		if (false == m_IsAnimLoopFinished)
		{
			IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
			if (IsAnimFinished)
				m_IsAnimLoopFinished = true;
		}
	}
	/* 그 외엔 상관 X */
	else 
		IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);

	if(ANIM_STATE::DOUBLE_JUMP == m_eAnimState || ANIM_STATE::FALL == m_eAnimState)
	{
		m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION) + XMVectorSet(0.f, m_fMovement, 0.f, 0.f));

		if (m_pGameInstance->Key_Pressing(DIK_W) ||
			m_pGameInstance->Key_Pressing(DIK_A) ||
			m_pGameInstance->Key_Pressing(DIK_D)
			)
		{
			m_pPlayer->Get_Transform()->Go_Straight(fTimeDelta * 1.5f, nullptr);

			if (m_pGameInstance->Key_Pressing(DIK_D))
				m_pPlayer->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 0.3f);

			if (m_pGameInstance->Key_Pressing(DIK_A))
				m_pPlayer->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -0.3f);
		}
	}
	else
	{
		m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION) + XMVectorSet(0.f, m_fMovement, 0.f, 0.f));

		if (m_pGameInstance->Key_Pressing(DIK_W) ||
			m_pGameInstance->Key_Pressing(DIK_A) ||
			m_pGameInstance->Key_Pressing(DIK_D))
		{

			m_pPlayer->Get_Transform()->Go_Straight(fTimeDelta * 1.5f, nullptr);

			if (m_pGameInstance->Key_Pressing(DIK_D))
				m_pPlayer->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 0.3f);

			if (m_pGameInstance->Key_Pressing(DIK_A))
				m_pPlayer->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -0.3f);
		}
	}
	// 낙하
	// 점프 중에 가속도 떨어지거나, 더블점프 중 + 애니 재생 끝났다면
	if (((m_fMovement < 0.f && m_fTimeAcc != 0.f) || (ANIM_STATE::DOUBLE_JUMP == m_eAnimState && IsAnimFinished)))
	{
		m_pPlayer->Set_AnimIndex("CustomMan_Fall_Front_Loop", 2.5f, true);
		m_eAnimState = ANIM_STATE::FALL;

	}
	// 더블 점프.
	if (m_pGameInstance->Key_Down(DIK_SPACE) && false == m_IsTriggered)
	{
		m_pPlayer->Set_Pickable(true);
		//보간 ratio 추가
		m_pPlayer->Set_AnimIndex("CustomMan_DoubleJump", 2.5f, false);
		m_fTimeAcc = 0.f;
		m_eAnimState = ANIM_STATE::DOUBLE_JUMP;
		m_IsTriggered = true;
	}

	// LAND로의 상태 전환.
	_bool  IsGround = m_pGameInstance->Check_GameObject_GeometryCollision(m_pPlayer);

	if (true == IsGround)
	{
		pNextState = CPlayer_LandState::Create(m_pPlayer);
		_float4 PlayerPos = {};
		XMStoreFloat4(&PlayerPos, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));
	}


	return pNextState;
}

_bool CPlayer_SuperJumpState::End()
{
	m_pPlayer->Set_Ground(true);

	return true;
}

CPlayer_SuperJumpState* CPlayer_SuperJumpState::Create(CPlayer* pPlayer, _float fPower)
{
	return new CPlayer_SuperJumpState(pPlayer, fPower);
}

void CPlayer_SuperJumpState::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
