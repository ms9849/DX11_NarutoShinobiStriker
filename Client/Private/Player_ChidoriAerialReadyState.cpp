#include "Player_ChidoriAerialReadyState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_ChidoriAttackState.h"

#pragma endregion

CPlayer_ChidoriAerialReadyState::CPlayer_ChidoriAerialReadyState(CPlayer* pPlayer, _float fTimeAcc)
	: m_pPlayer{ pPlayer }
	, m_fTimeAcc { fTimeAcc }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_ChidoriAerialReadyState::Start(_bool IsBlend)
{
	m_pPlayer->Set_Ground(false);
	m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Aerial_Chidori_Charge_Lv1", 1.f, true);
	m_eAnimState = ANIM_STATE::CHARGE_1;
}

CPlayerState* CPlayer_ChidoriAerialReadyState::Update(_float fTimeDelta)
{
	CPlayerState* pNewState = { nullptr };

	_bool IsGround = false;
	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pPlayer->Get_AnimProgress();

	m_fTimeAcc += fTimeDelta;
	m_fMovement = (m_fTimeAcc - 0.5f * m_fTimeAcc * m_fTimeAcc * 7.0f * (m_fTimeAcc));
	m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION) + XMVectorSet(0.f, m_fMovement / 5.f, 0.f, 0.f));


	//_float fHeight = m_pPlayer->Get_Navigation()->Get_CellHeight(m_pPlayer->Get_Transform());
	//if (XMVectorGetY(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION)) < fHeight)
	//{
	//	IsGround = true;
	//	_float4 PlayerPos = {};
	//	XMStoreFloat4(&PlayerPos, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));

	//	m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(PlayerPos.x, fHeight, PlayerPos.z, 1.f));
	//}

	if ((m_pGameInstance->Key_Pressing(DIK_W) ||
		m_pGameInstance->Key_Pressing(DIK_A) ||
		m_pGameInstance->Key_Pressing(DIK_D))
		&& false == IsGround)
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
			m_pPlayer->Get_Transform()->Go_Straight(fTimeDelta * 0.3f);

		if (m_pGameInstance->Key_Pressing(DIK_D))
			m_pPlayer->Get_Transform()->Go_Right(fTimeDelta * 0.3f);

		if (m_pGameInstance->Key_Pressing(DIK_A))
			m_pPlayer->Get_Transform()->Go_Left(fTimeDelta * 0.3f);
	}


	// 차지 1단계 애니메이션 재생이 끝났다면
	if (true == IsAnimFinished && ANIM_STATE::CHARGE_1 == m_eAnimState)
	{
		m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Aerial_Chidori_Charge_Lv2toLv3", 1.f, true);
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

_bool CPlayer_ChidoriAerialReadyState::End()
{
	m_pPlayer->Set_Ground(true);

	return true;
}

CPlayer_ChidoriAerialReadyState* CPlayer_ChidoriAerialReadyState::Create(CPlayer* pPlayer, _float fTimeAcc)
{
	return new CPlayer_ChidoriAerialReadyState(pPlayer, fTimeAcc);
}

void CPlayer_ChidoriAerialReadyState::Free()
{
	__super::Free();
	 
	Safe_Release(m_pPlayer);
}
