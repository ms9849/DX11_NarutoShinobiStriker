#include "Player_FrontJumpState.h"

#include "Player.h"
#include "GameInstance.h"
/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_LandState.h"
#include "Player_HandAerialAttackState.h"
#include "Player_ChidoriAerialReadyState.h"
#include "Player_AerialFireBallState.h"

#pragma endregion

CPlayer_FrontJumpState::CPlayer_FrontJumpState(CPlayer* pPlayer)
    : m_pPlayer { pPlayer }
{
    Safe_AddRef(m_pPlayer);
}

void CPlayer_FrontJumpState::Start(_bool IsBlend)
{
    m_pPlayer->Set_AnimIndex("CustomMan_Jump_Front", 1.f, true);
    m_eAnimState = ANIM_STATE::JUMP;
    m_bCanDoubleJump = true;
}

CPlayerState* CPlayer_FrontJumpState::Update(_float fTimeDelta)
{
    CPlayerState* pNextState = { nullptr };

	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);

	m_fTimeAcc += fTimeDelta;
	m_fMovement = (m_fTimeAcc - 0.5 * m_fTimeAcc * m_fTimeAcc * 7.0f * (m_fTimeAcc));
	m_pPlayer->Get_PlayerTransformPtr()->Set_State(STATE::POSITION, m_pPlayer->Get_PlayerTransformPtr()->Get_State(STATE::POSITION) + XMVectorSet(0.f, m_fMovement, 0.f, 0.f));

	if (m_pGameInstance->Key_Pressing(DIK_W) ||
		m_pGameInstance->Key_Pressing(DIK_A) ||
		m_pGameInstance->Key_Pressing(DIK_D)
		)
	{
		m_pPlayer->Get_PlayerTransformPtr()->Go_Straight(fTimeDelta);

		if (m_pGameInstance->Key_Pressing(DIK_D))
			m_pPlayer->Get_PlayerTransformPtr()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 0.3f);

		if (m_pGameInstance->Key_Pressing(DIK_A))
			m_pPlayer->Get_PlayerTransformPtr()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -0.3f);
	}


	// 더블 점프.
	if (m_pGameInstance->Key_Down(DIK_SPACE) && m_bCanDoubleJump && ANIM_STATE::JUMP == m_eAnimState && m_fTimeAcc >= 0.15f)
	{
		//보간 ratio 추가
		m_pPlayer->Set_AnimIndex("CustomMan_DoubleJump", 2.5f, false);
		m_eAnimState = ANIM_STATE::DOUBLE_JUMP;
		m_fTimeAcc = 0.f;
		m_bCanDoubleJump = false;
	}

	// 낙하
	// 점프 중에 가속도 떨어지거나, 더블점프 중 + 애니 재생 끝났다면
	if ((m_fMovement < 0.f && m_fTimeAcc != 0.f && ANIM_STATE::JUMP == m_eAnimState)
		|| (ANIM_STATE::DOUBLE_JUMP == m_eAnimState && IsAnimFinished))
	{
		m_pPlayer->Set_AnimIndex("CustomMan_Fall_Vertical_Loop", 1.0f, true);
		m_eAnimState = ANIM_STATE::FALL;
	}

	// LAND로의 상태 전환 
	if (XMVectorGetY(m_pPlayer->Get_PlayerTransformPtr()->Get_State(STATE::POSITION)) < 0.f)
	{
		pNextState = CPlayer_LandState::Create(m_pPlayer);
		_float4 PlayerPos = {};
		XMStoreFloat4(&PlayerPos, m_pPlayer->Get_PlayerTransformPtr()->Get_State(STATE::POSITION));
		m_pPlayer->Get_PlayerTransformPtr()->Set_State(STATE::POSITION, XMVectorSet(PlayerPos.x, 0, PlayerPos.z, 1.f));
	}
	// 공격
	else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON))
	{
		if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_HandAerialAttackState::Create(m_pPlayer, m_fTimeAcc);
		else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_HandAerialAttackState::Create(m_pPlayer, m_fTimeAcc);
	}
	/* 1번 스킬 사용 */
	else if (m_pGameInstance->Key_Down(DIK_1))
	{
		if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
			int a = 10;
			/*pNextState = CPlayer_HandAerialAttackState::Create(m_pPlayer, m_fTimeAcc);*/
		else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_ChidoriAerialReadyState::Create(m_pPlayer, m_fTimeAcc);
	}
	/* 2번 스킬 사용 */
	else if (m_pGameInstance->Key_Down(DIK_2))
	{
		// 근접 타입 
		if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
			int a = 10;
		//pNextState = CPlayer_HandAttackState::Create(m_pPlayer);
		// 인술 타입 
		else if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
		{
			pNextState = CPlayer_AerialFireBallState::Create(m_pPlayer, m_fTimeAcc);
		}
	}

	return pNextState;
}

_bool CPlayer_FrontJumpState::End()
{
	return true;
}

CPlayer_FrontJumpState* CPlayer_FrontJumpState::Create(CPlayer* pPlayer)
{
    return new CPlayer_FrontJumpState(pPlayer);
}

void CPlayer_FrontJumpState::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
}
