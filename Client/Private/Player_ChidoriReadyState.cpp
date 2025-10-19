#include "Player_ChidoriReadyState.h"

#include "Player.h"
#include "GameInstance.h"
#include "Chidori.h"

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
	m_pPlayer->Set_Invincible(true);
	m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Chidori_Charge_Lv1", 0.5f, true);
	m_eAnimState = ANIM_STATE::CHARGE_1;
}

CPlayerState* CPlayer_ChidoriReadyState::Update(_float fTimeDelta)
{
	CPlayerState* pNewState = { nullptr };

	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pPlayer->Get_AnimProgress();

	if (m_eAnimState == ANIM_STATE::CHARGE_1 && fAnimProgress >= 0.25f && false == m_IsChidoriCreated)
	{
		CChidori::CHIDORI_DESC Desc;
		Desc.pSocketMatrix = m_pPlayer->Get_BoneMatrix(TEXT("Part_Upper"), "RightHandMiddle1");

		m_pChidori = static_cast<CChidori*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC),
			TEXT("Prototype_GameObject_Chidori"), &Desc));

		m_pGameInstance->Add_Clone_ToLayer(m_pChidori, m_pGameInstance->Get_LevelID(), TEXT("Layer_Skill"));
		Safe_AddRef(m_pChidori);

		m_IsChidoriCreated = true;
	}

	// 차지 1단계 애니메이션 재생이 끝났다면
	if (true == IsAnimFinished && ANIM_STATE::CHARGE_1 == m_eAnimState)
	{
		m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Chidori_Charge_Lv2toLv3", 0.25f, true);
		m_eAnimState = ANIM_STATE::CHARGE_2;
	}
	// 차지 2단계 애니메이션 재생이 끝났다면
	if (true == IsAnimFinished && ANIM_STATE::CHARGE_2 == m_eAnimState)
	{
		m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Aerial_Chidori_Charge_Lv2toLv3_Loop", 0.25f, true);
		m_eAnimState = ANIM_STATE::CHARGE_3;
	}

	// 치도리 공격 상태
	if (true == IsAnimFinished && ANIM_STATE::CHARGE_3 == m_eAnimState)
	{
		pNewState = CPlayer_ChidoriAttackState::Create(m_pPlayer, m_pChidori);
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
	__super::Free();

	Safe_Release(m_pPlayer);
	Safe_Release(m_pChidori);
}
