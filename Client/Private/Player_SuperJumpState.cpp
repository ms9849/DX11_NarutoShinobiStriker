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
	m_pPlayer->Set_AnimIndex("CustomMan_ChakraJump_Charge_End", 0.6f, false);
	m_eAnimState = ANIM_STATE::START;
}

CPlayerState* CPlayer_SuperJumpState::Update(_float fTimeDelta)
{
	CPlayerState* pNextState = { nullptr };
	_bool IsAnimFinished; 
	_float fAnimProgress = m_pPlayer->Get_AnimProgress();

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

	m_fTimeAcc += fTimeDelta;
	m_fMovement = (m_fPower * 1.5f * m_fTimeAcc - 0.5f * m_fTimeAcc * m_fTimeAcc * 7.0f * (m_fTimeAcc));
	if (m_fMovement <= -0.5f)
		m_fMovement = -0.5f;

	m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION) + XMVectorSet(0.f, m_fMovement, 0.f, 0.f));
	m_pPlayer->Get_Transform()->Go_Straight(fTimeDelta * 2.5f);

	if (m_pGameInstance->Key_Pressing(DIK_A) ||
		m_pGameInstance->Key_Pressing(DIK_D)
		)
	{

		if (m_pGameInstance->Key_Pressing(DIK_D))
			m_pPlayer->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 0.3f);

		if (m_pGameInstance->Key_Pressing(DIK_A))
			m_pPlayer->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -0.3f);
	}

	if (m_fMovement < 0.f && ANIM_STATE::START == m_eAnimState)
	{
		m_pPlayer->Set_AnimIndex("CustomMan_Jump_Vertical", 1.f, true, 0.2f);
		m_eAnimState = ANIM_STATE::JUMP; 
	}

	// LAND로의 상태 전환 
	if (XMVectorGetY(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION)) < 0.f)
	{
		pNextState = CPlayer_LandState::Create(m_pPlayer);
		_float4 PlayerPos = {};
		XMStoreFloat4(&PlayerPos, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));
		m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(PlayerPos.x, 0, PlayerPos.z, 1.f));
	}

	return pNextState;
}

_bool CPlayer_SuperJumpState::End()
{
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
