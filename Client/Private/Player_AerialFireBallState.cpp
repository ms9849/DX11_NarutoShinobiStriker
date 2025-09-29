#include "Player_AerialFireBallState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"
#include "Player_JumpState.h"

#pragma endregion

CPlayer_AerialFireBallState::CPlayer_AerialFireBallState(CPlayer* pPlayer, _float fTimeAcc)
	: m_pPlayer { pPlayer}
	, m_fTimeAcc { fTimeAcc }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_AerialFireBallState::Start(_bool IsBlend)
{
	m_pPlayer->Set_Ground(false);
	m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Aerial_Fireball_Lv3", 1.75f, true);
}

CPlayerState* CPlayer_AerialFireBallState::Update(_float fTimeDelta)
{
	CPlayerState* pNextState = { nullptr };

	_bool IsGround = false;
	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pPlayer->Get_AnimProgress();

	m_fTimeAcc += fTimeDelta;

	if (fAnimProgress <= 0.4f || fAnimProgress >= 0.5f)
	{
		m_fCurMovement = (m_fTimeAcc - 0.5f * m_fTimeAcc * m_fTimeAcc * 7.0f * (m_fTimeAcc));
		m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION) + XMVectorSet(0.f, m_fCurMovement / 5.f, 0.f, 0.f));
	}
	else if (fAnimProgress > 0.4f && fAnimProgress < 0.5f)
	{
		if (false == m_IsFloat)
		{
			m_fTimeAcc = 0.f;
			m_IsFloat = true;
		}

		m_fCurMovement = (m_fTimeAcc - 0.5f * m_fTimeAcc * m_fTimeAcc * 7.0f * (m_fTimeAcc));
		m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION) + XMVectorSet(0.f, fTimeDelta, 0.f, 0.f));
	}

	//_float fHeight = m_pPlayer->Get_Navigation()->Get_CellHeight(m_pPlayer->Get_Transform());
	//if (XMVectorGetY(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION)) < fHeight)
	//{
	//	IsGround = true;
	//	_float4 PlayerPos = {};
	//	XMStoreFloat4(&PlayerPos, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));
	//	m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(PlayerPos.x, fHeight, PlayerPos.z, 1.f));
	//}

	//if (false == IsGround)
	//{
	//	if (m_pGameInstance->Key_Pressing(DIK_W))
	//		m_pPlayer->Get_Transform()->Go_Straight(fTimeDelta * 0.3f, m_pPlayer->Get_Navigation());
	//}

	if (fAnimProgress > 0.8f && XMVectorGetY(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION)) > 0.f)
	{
		pNextState = CPlayer_JumpState::Create(m_pPlayer, 0.55f, CPlayer_JumpState::ANIM_STATE::FALL);
	}
	else if(fAnimProgress > 0.8f && XMVectorGetY(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION)) <= 0.f)
	{
		_float4 PlayerPos = {};
		XMStoreFloat4(&PlayerPos, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));
		m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(PlayerPos.x, 0, PlayerPos.z, 1.f));
		pNextState = CPlayer_IdleState::Create(m_pPlayer);
	}

	return pNextState;
}

_bool CPlayer_AerialFireBallState::End()
{
	m_pPlayer->Set_Ground(true);

	return true;
}

CPlayer_AerialFireBallState* CPlayer_AerialFireBallState::Create(CPlayer* pPlayer, _float fTimeAcc)
{
	return new CPlayer_AerialFireBallState(pPlayer, fTimeAcc);
}

void CPlayer_AerialFireBallState::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
