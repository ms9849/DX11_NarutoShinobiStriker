#include "Player_BeatenState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"
#include "Player_StepState.h"

#pragma endregion

CPlayer_BeatenState::CPlayer_BeatenState(CPlayer* pPlayer, _vector vDir, _float fRatio)
	: m_pPlayer { pPlayer }
	, m_fRatio { fRatio }
{
	Safe_AddRef(m_pPlayer);
	XMStoreFloat3(&m_vDirection, vDir);
}

void CPlayer_BeatenState::Start(_bool IsBlend)
{
	m_pPlayer->Set_AnimIndex("CustomMan_Beaten_Left", 1.f, IsBlend);

	if (TEXT("CustomMan_Beaten_Left") != m_pPlayer->Get_CurrentAnim())
		m_pPlayer->Set_AnimIndex("CustomMan_Beaten_Left", 1.5f, IsBlend);
	else
		m_pPlayer->Set_AnimIndex("CustomMan_Beaten_Right", 1.5f, IsBlend);

}

CPlayerState* CPlayer_BeatenState::Update(_float fTimeDelta)
{
	CPlayerState* pNextState = { nullptr };

	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pPlayer->Get_AnimProgress();

	/* 임시 코드. 방향 받아와서 처리해야 한다.*/
	if (false == IsAnimFinished && fAnimProgress <= 0.7f)
		m_pPlayer->Get_Transform()->Go_Direction(XMVectorSetY(XMLoadFloat3(&m_vDirection), 0.f), m_fRatio * fTimeDelta * m_pGameInstance->Calc_Quadratic(-0.89f, 0.48f, 0.11f, fAnimProgress),
			nullptr);

	if (true == IsAnimFinished)
	{
		pNextState = CPlayer_IdleState::Create(m_pPlayer);
	}

	// 백스텝
	else if (m_pGameInstance->Key_Pressing(DIK_S) && m_pGameInstance->Key_Down(DIK_LSHIFT)
		&& fAnimProgress >= 0.6f)
	{
		pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::BACK);
		m_IsNextAnimBlened = true;
	}
	// 오른쪽 스텝
	else if (m_pGameInstance->Key_Pressing(DIK_D)
		&& fAnimProgress >= 0.6f)
	{
		if (m_pGameInstance->Key_Down(DIK_LSHIFT) && nullptr == pNextState)
			pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::RIGHT);
	}
	// 왼쪽 스텝
	else if (m_pGameInstance->Key_Pressing(DIK_A)
		&& fAnimProgress >= 0.6f)
	{
		if (m_pGameInstance->Key_Down(DIK_LSHIFT) && nullptr == pNextState)
			pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::LEFT);
	}

	return pNextState;
}

_bool CPlayer_BeatenState::End()
{
	return true;
}

CPlayer_BeatenState* CPlayer_BeatenState::Create(CPlayer* pPlayer, _vector vDir, _float fRatio)
{
	return new CPlayer_BeatenState(pPlayer, vDir, fRatio);
}

void CPlayer_BeatenState::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
