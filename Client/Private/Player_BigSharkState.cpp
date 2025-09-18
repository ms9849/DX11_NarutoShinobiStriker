#include "Player_BigSharkState.h"

#include "Player.h"
#include "GameInstance.h"
#include "BigShark.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"

#pragma endregion


CPlayer_BigSharkState::CPlayer_BigSharkState(CPlayer* pPlayer)
	: m_pPlayer { pPlayer }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_BigSharkState::Start(_bool IsBlend)
{
	m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_SuperSharkBomb", 1.5f, IsBlend, 0.1f);
}

CPlayerState* CPlayer_BigSharkState::Update(_float fTimeDelta)
{
	CPlayerState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pPlayer->Get_AnimProgress();

	if (fAnimProgress > 0.8f && false == m_isBigSharkThrow)
	{
		CBigShark::BIGSHARK_DESC Desc;

		Desc.fSpeedPerSec = 20.f;
		XMStoreFloat3(&Desc.vPosition, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));
		XMStoreFloat3(&Desc.vLook, m_pPlayer->Get_Transform()->Get_State(STATE::LOOK));

		m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_BigShark"),
			m_pGameInstance->Get_LevelID(), TEXT("Layer_Skill"), &Desc);

		m_isBigSharkThrow = true;
	}

	if (true == IsAnimFinished)
		pNextState = CPlayer_IdleState::Create(m_pPlayer);

	return pNextState;
}

_bool CPlayer_BigSharkState::End()
{
	return true;
}

CPlayer_BigSharkState* CPlayer_BigSharkState::Create(CPlayer* pPlayer)
{
	return new CPlayer_BigSharkState(pPlayer);
}

void CPlayer_BigSharkState::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
