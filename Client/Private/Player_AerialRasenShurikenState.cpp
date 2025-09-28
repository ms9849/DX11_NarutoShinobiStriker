#include "Player_AerialRasenShurikenState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"
#include "Player_JumpState.h"

#pragma endregion

CPlayer_AerialRasenShurikenState::CPlayer_AerialRasenShurikenState(CPlayer* pPlayer, _float fTimeAcc)
	: m_pPlayer { pPlayer }
	, m_fTimeAcc { fTimeAcc }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_AerialRasenShurikenState::Start(_bool IsBlend)
{
	m_pPlayer->Set_Ground(false);
	m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Aerial_TrueRasenShuriken", 1.5f, IsBlend);
}

CPlayerState* CPlayer_AerialRasenShurikenState::Update(_float fTimeDelta)
{
	CPlayerState* pNextState = { nullptr };

	_bool IsGround = false;
	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pPlayer->Get_AnimProgress();

	m_fTimeAcc += fTimeDelta;

	if (fAnimProgress < 0.3f)
	{
		m_fMovement = (m_fTimeAcc - 0.5f * m_fTimeAcc * m_fTimeAcc * 7.0f * (m_fTimeAcc));
		m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION) + XMVectorSet(0.f, 0.05f * m_fMovement * (0.3f - fAnimProgress), 0.f, 0.f));
	}

	//_float fHeight = m_pPlayer->Get_Navigation()->Get_CellHeight(m_pPlayer->Get_Transform());
	//if (XMVectorGetY(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION)) < fHeight)
	//{
	//	IsGround = true;
	//	_float4 PlayerPos = {};
	//	XMStoreFloat4(&PlayerPos, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));
	//	m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(PlayerPos.x, fHeight, PlayerPos.z, 1.f));
	//}

	if (fAnimProgress > 0.8f && XMVectorGetY(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION)) > 0.f)
	{
		pNextState = CPlayer_JumpState::Create(m_pPlayer, 0.55f, CPlayer_JumpState::ANIM_STATE::FALL);
	}
	else if (fAnimProgress > 0.8f && XMVectorGetY(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION)) <= 0.f)
	{
		_float4 PlayerPos = {};
		XMStoreFloat4(&PlayerPos, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));
		m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(PlayerPos.x, 0, PlayerPos.z, 1.f));
		pNextState = CPlayer_IdleState::Create(m_pPlayer);
	}

	return pNextState;
}

_bool CPlayer_AerialRasenShurikenState::End()
{
	m_pPlayer->Set_Ground(true);

	return true;
}

CPlayer_AerialRasenShurikenState* CPlayer_AerialRasenShurikenState::Create(CPlayer* pPlayer, _float fTimeAcc)
{
	return new CPlayer_AerialRasenShurikenState(pPlayer, fTimeAcc);
}

void CPlayer_AerialRasenShurikenState::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
