#include "Player_ChidoriReadyState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_ChidoriAttackState.h"

#pragma endregion

CPlayer_ChidoriReadyState::CPlayer_ChidoriReadyState(CPlayer* pPlayer)
	: m_pPlayer { pPlayer }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_ChidoriReadyState::Start(_bool IsBlend)
{
	m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Chidori_Charge_Lv1", 1.f, true);
	m_eAnimState = ANIM_STATE::CHARGE_1;
}

CPlayerState* CPlayer_ChidoriReadyState::Update(_float fTimeDelta)
{
	CPlayerState* pNewState = { nullptr };

	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pPlayer->Get_AnimProgress();

	// 차지 1단계 애니메이션 재생이 끝났다면
	if (true == IsAnimFinished && ANIM_STATE::CHARGE_1 == m_eAnimState)
	{
		m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Chidori_Charge_Lv2toLv3", 1.f, true);
		m_eAnimState = ANIM_STATE::CHARGE_2;
	}
	// 차지 2단계 애니메이션 재생이 끝났다면
	if (true == IsAnimFinished && ANIM_STATE::CHARGE_2 == m_eAnimState)
	{
		m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Aerial_Chidori_Charge_Lv2toLv3_Loop", 1.f, true);
		m_eAnimState = ANIM_STATE::CHARGE_3;
	}

	// 치도리 공격 상태
	if (true == IsAnimFinished && ANIM_STATE::CHARGE_3 == m_eAnimState)
	{
		pNewState = CPlayer_ChidoriAttackState::Create(m_pPlayer);
	}

	return pNewState;
}

_bool CPlayer_ChidoriReadyState::End()
{
	return true;
}

CPlayer_ChidoriReadyState* CPlayer_ChidoriReadyState::Create(CPlayer* pPlayer)
{
	return new CPlayer_ChidoriReadyState(pPlayer);
}

void CPlayer_ChidoriReadyState::Free()
{
	Safe_Release(m_pPlayer);
}
