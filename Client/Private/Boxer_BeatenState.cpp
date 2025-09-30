#include "Boxer_BeatenState.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Boxer.h"

#pragma region TRANSFER_STATE

#include "Boxer_IdleState.h"

#pragma endregion

CBoxer_BeatenState::CBoxer_BeatenState(CNavigation* pNavigation, CBoxer* pBoxer, _vector vDir, _float fRatio)
	: m_pBoxer { pBoxer }
	, m_pNavigationCom{ pNavigation }
	, m_fRatio{ fRatio }
{
	Safe_AddRef(m_pNavigationCom);
	XMStoreFloat3(&m_vDirection, vDir);
}

void CBoxer_BeatenState::Start(_bool IsBlend)
{
	if (TEXT("CustomMan_Beaten_Left") != m_pBoxer->Get_CurrentAnim())
		m_pBoxer->Set_AnimIndex("CustomMan_Beaten_Left", 1.5f, false);
	else
		m_pBoxer->Set_AnimIndex("CustomMan_Beaten_Right", 1.5f, false);

	m_pBoxer->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
}

CBoxerState* CBoxer_BeatenState::Update(_float fTimeDelta)
{
	CBoxerState* pNextState = { nullptr };

	_bool IsAnimFinished = m_pBoxer->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pBoxer->Get_AnimProgress();

	/* 임시 코드. 방향 받아와서 처리해야 한다.*/
	if (false == IsAnimFinished && fAnimProgress <= 0.7f)
		m_pBoxer->Get_Transform()->Go_Direction(XMVectorSetY(XMLoadFloat3(&m_vDirection), 0.f), m_fRatio * fTimeDelta * m_pGameInstance->Calc_Quadratic(-0.89f, 0.48f, 0.11f, fAnimProgress),
			m_pNavigationCom);

	if (true == IsAnimFinished)
	{
		pNextState = CBoxer_IdleState::Create(m_pNavigationCom, m_pBoxer);
	}

	return pNextState;
}

_bool CBoxer_BeatenState::End()
{
	return true;
}

CBoxer_BeatenState* CBoxer_BeatenState::Create(CNavigation* pNavigation, CBoxer* pBoxer, _vector vDir, _float fRatio)
{
	return new CBoxer_BeatenState(pNavigation, pBoxer, vDir, fRatio);
}

void CBoxer_BeatenState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
