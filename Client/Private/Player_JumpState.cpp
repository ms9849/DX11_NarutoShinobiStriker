#include "Player_JumpState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"

#pragma endregion

CPlayer_JumpState::CPlayer_JumpState(CPlayer* pPlayer)
    : m_pPlayer { pPlayer }
{
    Safe_AddRef(m_pPlayer);
}

void CPlayer_JumpState::Start()
{
    m_pPlayer->Set_AnimIndex("CustomMan_Jump_Vertical", 1.f, true);
	m_eAnimState = ANIM_STATE::JUMP;
	m_bCanDoubleJump = true;
}

CPlayerState* CPlayer_JumpState::Update(_float fTimeDelta)
{
	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);

	m_fTimeAcc += fTimeDelta;
	m_fMovement = (m_fTimeAcc - 0.5 * m_fTimeAcc * m_fTimeAcc * 7.0f * (m_fTimeAcc));
	m_pPlayer->Get_PlayerTransformPtr()->Set_State(STATE::POSITION, m_pPlayer->Get_PlayerTransformPtr()->Get_State(STATE::POSITION) + XMVectorSet(0.f, m_fMovement, 0.f, 0.f));

	CPlayerState* pNextState = { nullptr };

	// 더블 점프.
	if (m_pGameInstance->Key_Down(DIK_SPACE) && m_bCanDoubleJump)
	{
		m_pPlayer->Set_AnimIndex("CustomMan_DoubleJump", 4.0f, false);
		m_eAnimState = ANIM_STATE::DOUBLE_JUMP;

		m_fTimeAcc = 0.f;
		m_fMovement = 0.f;

		m_bCanDoubleJump = false;
	}
	// 낙하

	// 점프 중에 가속도 떨어지거나, 더블점프 중 + 애니 재생 끝났다면
	if ((m_fMovement < 0.f && m_fTimeAcc != 0.f && ANIM_STATE::FALL != m_eAnimState) 
		|| (ANIM_STATE::DOUBLE_JUMP == m_eAnimState && IsAnimFinished))
	{
		m_pPlayer->Set_AnimIndex("CustomMan_Fall_Vertical_Loop", 1.0f, true);
		m_eAnimState = ANIM_STATE::FALL;
	}

	// 낙법. LAND는 아직 하지말고 일단 변경..
	if (XMVectorGetY(m_pPlayer->Get_PlayerTransformPtr()->Get_State(STATE::POSITION)) < 0.f)
	{
		pNextState = CPlayer_IdleState::Create(m_pPlayer);
		_float4 PlayerPos = {};
		XMStoreFloat4(&PlayerPos, m_pPlayer->Get_PlayerTransformPtr()->Get_State(STATE::POSITION));
		m_pPlayer->Get_PlayerTransformPtr()->Set_State(STATE::POSITION, XMVectorSet(PlayerPos.x, 0, PlayerPos.z, 1.f));
	}

	return pNextState;
}

void CPlayer_JumpState::End()
{
}

CPlayer_JumpState* CPlayer_JumpState::Create(CPlayer* pPlayer)
{
	return new CPlayer_JumpState(pPlayer);
}

void CPlayer_JumpState::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
