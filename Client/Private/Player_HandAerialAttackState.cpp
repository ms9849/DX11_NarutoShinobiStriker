#include "Player_HandAerialAttackState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_JumpState.h"
#include "Player_LandState.h"

#pragma endregion

/* TimeACc 이어서 쌓기 */

CPlayer_HandAerialAttackState::CPlayer_HandAerialAttackState(CPlayer* pPlayer, _float fTimeAcc)
	: m_pPlayer { pPlayer }
    , m_fTimeAcc { fTimeAcc }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_HandAerialAttackState::Start(_bool IsBlend)
{
	m_pPlayer->Set_AnimIndex("CustomMan_Attack_Aerial_cmb01", 1.5f, IsBlend);
	m_eAnimState = ANIM_STATE::ATTACK_01;
}

CPlayerState* CPlayer_HandAerialAttackState::Update(_float fTimeDelta)
{
    CPlayerState* pNextState = { nullptr };

	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pPlayer->Get_AnimProgress();

	if (m_pGameInstance->Key_Pressing(DIK_W) ||
		m_pGameInstance->Key_Pressing(DIK_A) ||
		m_pGameInstance->Key_Pressing(DIK_D)
		)
	{
		m_pPlayer->Get_Transform()->Go_Straight(fTimeDelta * 0.4f);

		if (m_pGameInstance->Key_Pressing(DIK_D))
			m_pPlayer->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 0.3f);

		if (m_pGameInstance->Key_Pressing(DIK_A))
			m_pPlayer->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -0.3f);
	}

    if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON)
        && m_eAnimState == ANIM_STATE::ATTACK_01 && fAnimProgress >= 0.7f)
    {
        m_pPlayer->Set_AnimIndex("CustomMan_Attack_Aerial_cmb04", 1.8f, true);
        m_eAnimState = ANIM_STATE::ATTACK_02;
    }

	// LAND로의 상태 전환 
	if (XMVectorGetY(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION)) <= 0.f)
	{
		pNextState = CPlayer_LandState::Create(m_pPlayer);
		_float4 PlayerPos = {};
		XMStoreFloat4(&PlayerPos, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));
		m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(PlayerPos.x, 0, PlayerPos.z, 1.f));
	}

	//낙하로 넘어가게 한다.
	else if (true == IsAnimFinished)
	{
		pNextState = CPlayer_JumpState::Create(m_pPlayer, 0.57f, CPlayer_JumpState::ANIM_STATE::FALL);
	}

	_float fMovement = 0.02f * m_pGameInstance->Calc_Linear(-8.f, 4.f, fAnimProgress);

	m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION) + XMVectorSet(0.f, fMovement, 0.f, 0.f));


    return pNextState;
}

_bool CPlayer_HandAerialAttackState::End()
{
	return true;
}

CPlayer_HandAerialAttackState* CPlayer_HandAerialAttackState::Create(CPlayer* pPlayer, _float fTimeAcc)
{
	return new CPlayer_HandAerialAttackState(pPlayer, fTimeAcc);
}

void CPlayer_HandAerialAttackState::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
