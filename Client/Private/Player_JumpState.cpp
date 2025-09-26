#include "Player_JumpState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_LandState.h"
#include "Player_HandAerialAttackState.h"

#include "Player_ChidoriAerialReadyState.h"
#include "Player_AerialFireBallState.h"

#include "Player_AerialRasenShurikenState.h"
#pragma endregion

CPlayer_JumpState::CPlayer_JumpState(CPlayer* pPlayer, _float fTimeAcc, ANIM_STATE eStartAnimState)
    : m_pPlayer { pPlayer }
	, m_fTimeAcc { fTimeAcc}
	, m_eAnimState { eStartAnimState }
{
    Safe_AddRef(m_pPlayer);
}

void CPlayer_JumpState::Start(_bool IsBlend)
{
	m_pPlayer->Set_Ground(false);
	m_pPlayer->Set_Pickable(false);

	if (m_eAnimState == ANIM_STATE::JUMP)
	{
		m_pPlayer->Set_AnimIndex("CustomMan_Jump_Vertical", 1.f, IsBlend);
		m_bCanDoubleJump = true;
	}
	else if (m_eAnimState == ANIM_STATE::FALL)
	{
		m_pPlayer->Set_AnimIndex("CustomMan_Fall_Vertical_Loop", 1.0f, true);
		m_bCanDoubleJump = false;
	}
}

CPlayerState* CPlayer_JumpState::Update(_float fTimeDelta)
{
	if (m_fTimeAcc >= 0.35f)
		m_pPlayer->Set_Pickable(true);

	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);

	m_fTimeAcc += fTimeDelta;
	m_fMovement = (m_fTimeAcc - 0.5f * m_fTimeAcc * m_fTimeAcc * 7.0f * (m_fTimeAcc));
	if (m_fMovement <= -0.25f)
		m_fMovement = -0.25f;
	else if (m_fMovement >= 0.2f)
		m_fMovement = 0.2f;

	m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION) + XMVectorSet(0.f, m_fMovement, 0.f, 0.f));

	CPlayerState* pNextState = { nullptr };

	if (m_pGameInstance->Key_Pressing(DIK_W) ||
		m_pGameInstance->Key_Pressing(DIK_A) ||
		m_pGameInstance->Key_Pressing(DIK_D)
		)
	{
		m_pPlayer->Get_Transform()->Go_Straight(fTimeDelta,
			nullptr);

		if (m_pGameInstance->Key_Pressing(DIK_D))
			m_pPlayer->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 0.3f);

		if (m_pGameInstance->Key_Pressing(DIK_A))
			m_pPlayer->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -0.3f);
	}

	// 더블 점프.
	if (m_pGameInstance->Key_Down(DIK_SPACE) && m_bCanDoubleJump && m_fTimeAcc >= 0.15f)
	{
		//보간 ratio 추가
		m_pPlayer->Set_Pickable(true);
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
		m_pPlayer->Set_AnimIndex("CustomMan_Fall_Vertical_Loop", 2.5f, true);
		m_eAnimState = ANIM_STATE::FALL;
	}

	// LAND로의 상태 전환 
	_bool  IsGround = m_pGameInstance->Check_GameObject_GeometryCollision(m_pPlayer);

	if (true == IsGround)
	{
		pNextState = CPlayer_LandState::Create(m_pPlayer);
		_float4 PlayerPos = {};
		XMStoreFloat4(&PlayerPos, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));
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
	else if (m_pGameInstance->Key_Down(DIK_1) && m_pPlayer->Use_Skill(SKILLNUM::SECOND))
	{
		if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_ChidoriAerialReadyState::Create(m_pPlayer, m_fTimeAcc);
	}
	/* 2번 스킬 사용 */
	else if (m_pGameInstance->Key_Down(DIK_2) && m_pPlayer->Use_Skill(SKILLNUM::THIRD))
	{
		if (ATTACK_TYPE::NINJUTSU == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_AerialFireBallState::Create(m_pPlayer, m_fTimeAcc);

		else if (ATTACK_TYPE::MELEE == m_pPlayer->Get_AttackType())
			pNextState = CPlayer_AerialRasenShurikenState::Create(m_pPlayer, m_fTimeAcc);

	}
	return pNextState;
}

_bool CPlayer_JumpState::End()
{
	m_pPlayer->Set_Ground(true);
	m_pPlayer->Set_Pickable(true);

	return true;
}


CPlayer_JumpState* CPlayer_JumpState::Create(CPlayer* pPlayer, _float fTimeAcc, ANIM_STATE eStartAnimState)
{
	return new CPlayer_JumpState(pPlayer, fTimeAcc, eStartAnimState);
}

void CPlayer_JumpState::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
