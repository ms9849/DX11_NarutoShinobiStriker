#include "Player_FrontJumpState.h"

#include "Player.h"
#include "GameInstance.h"
#include "EffectObject.h"
#include "EffectContainer.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_LandState.h"
#include "Player_HandAerialAttackState.h"

#include "Player_AerialFireBallState.h"

#include "Player_AerialRasenShurikenState.h"
#include "Player_RopeActionState.h"

#pragma endregion

CPlayer_FrontJumpState::CPlayer_FrontJumpState(CPlayer* pPlayer, _float fTimeAcc, ANIM_STATE eStartAnimState)
	: m_pPlayer{ pPlayer }
	, m_fTimeAcc { fTimeAcc }
	, m_eAnimState { eStartAnimState }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_FrontJumpState::Start(_bool IsBlend)
{
	m_pPlayer->Set_Ground(false);
	m_pPlayer->Set_Pickable(false);

	if (m_eAnimState == ANIM_STATE::JUMP)
		m_pPlayer->Set_AnimIndex("CustomMan_Jump_Front", 1.f, IsBlend);


	else if (m_eAnimState == ANIM_STATE::FALL)
		m_pPlayer->Set_AnimIndex("CustomMan_Fall_Front_Loop", 1.0f, true);

	else if (m_eAnimState == ANIM_STATE::DOUBLE_JUMP)
	{
		m_pPlayer->Set_Pickable(false);
		//보간 ratio 추가
		m_pPlayer->Set_AnimIndex("CustomMan_DoubleJump", 2.5f, false);
		m_eAnimState = ANIM_STATE::DOUBLE_JUMP;
		m_fTimeAcc = 0.f;
		m_bCanDoubleJump = false;


		/* 아기상어뚜루루뚜루 */
		CEffectContainer::EFFECT_CONTAINER_DESC EffectDesc;
		EffectDesc.IsBinary = true;
		EffectDesc.strFilePath = TEXT("../Bin/Resources/Effects/Player_Jump_eff.bin");
		EffectDesc.fLifeTime = 0.6f;

		CEffectContainer* pCloneEffect = static_cast<CEffectContainer*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EffectContainer"),
			&EffectDesc));
		pCloneEffect->Set_Position(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));

		m_pGameInstance->Add_Clone_ToLayer(pCloneEffect, m_pGameInstance->Get_LevelID(), TEXT("Layer_Effect"));
	}
}

CPlayerState* CPlayer_FrontJumpState::Update(_float fTimeDelta)
{
	if (m_fTimeAcc >= 0.5f)
		m_pPlayer->Set_Pickable(true);

	else if (m_fTimeAcc >= 0.1f && m_eAnimState == ANIM_STATE::DOUBLE_JUMP)
		m_pPlayer->Set_Pickable(true);

    CPlayerState* pNextState = { nullptr };

	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);

	_float fStartSpeed = 6.f;    // 초기 점프 속도
	_float fGravity = 9.8f;    // 중력
	_float fJumpScale = 0.08f; // 전체 스케일 (최대 높이)

	m_fTimeAcc += fTimeDelta * 2.1f;

	m_fCurMovement = fStartSpeed * m_fTimeAcc - 0.5f * fGravity * m_fTimeAcc * m_fTimeAcc;
	m_fCurMovement *= fJumpScale;

	// 최솟값 제한
	if (m_fCurMovement < -0.3f)
		m_fCurMovement = -0.3f;

	// 이번 프레임의 이동량
	_float fDeltaMovement = m_fCurMovement - m_fPreMovement;

	// Q 누르면 이번 프레임의 이동량만 0.3배로 줄임
	if (m_pGameInstance->Key_Pressing(DIK_Q) && m_fTimeAcc >= 0.5f)
		fDeltaMovement *= 0.05f;

	// 최종 이동량.
	m_fCurMovement = m_fPreMovement + fDeltaMovement;

	// 다음 프레임을 위해 저장
	m_fPreMovement = m_fCurMovement;

	m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION) + XMVectorSet(0.f, m_fCurMovement, 0.f, 0.f));

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
		m_pPlayer->Set_Pickable(false);
		//보간 ratio 추가
		m_pPlayer->Set_AnimIndex("CustomMan_DoubleJump", 2.5f, false);
		m_eAnimState = ANIM_STATE::DOUBLE_JUMP;
		m_fTimeAcc = 0.f;
		m_bCanDoubleJump = false;

		/* 아기상어뚜루루뚜루 */
		CEffectContainer::EFFECT_CONTAINER_DESC EffectDesc;
		EffectDesc.IsBinary = true;
		EffectDesc.strFilePath = TEXT("../Bin/Resources/Effects/Player_Jump_eff.bin");
		EffectDesc.fLifeTime = 0.8f;

		CEffectContainer* pCloneEffect = static_cast<CEffectContainer*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EffectContainer"),
			&EffectDesc));
		pCloneEffect->Set_Position(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));

		m_pGameInstance->Add_Clone_ToLayer(pCloneEffect, m_pGameInstance->Get_LevelID(), TEXT("Layer_Effect"));
	}

	// 낙하
	// 점프 중에 가속도 떨어지거나, 더블점프 중 + 애니 재생 끝났다면
	if ((m_fCurMovement < 0.f && m_fTimeAcc != 0.f && ANIM_STATE::JUMP == m_eAnimState)
		|| (ANIM_STATE::DOUBLE_JUMP == m_eAnimState && IsAnimFinished))
	{
		m_pPlayer->Set_AnimIndex("CustomMan_Fall_Front_Loop", 1.25f, true);
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
	else if (m_pGameInstance->Key_Up(DIK_Q))
	{
		pNextState = CPlayer_RopeActionState::Create(m_pPlayer);
	}
	return pNextState;
}

_bool CPlayer_FrontJumpState::End()
{
	m_pPlayer->Set_Ground(true);
	m_pPlayer->Set_Pickable(true);

	return true;
}

CPlayer_FrontJumpState* CPlayer_FrontJumpState::Create(CPlayer* pPlayer, _float fTimeAcc, ANIM_STATE eStartAnimState)
{
	return new CPlayer_FrontJumpState(pPlayer, fTimeAcc, eStartAnimState);
}

void CPlayer_FrontJumpState::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
}
