#include "Pajama_BeatenState.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Pajama.h"

#pragma region TRANSFER_STATE

#include "Pajama_IdleState.h"

#pragma endregion

CPajama_BeatenState::CPajama_BeatenState(CNavigation* pNavigation, CPajama* pPajama, _vector vDir, _float fRatio)
	: m_pPajama { pPajama }
	, m_pNavigationCom{ pNavigation }
	, m_fRatio{ fRatio }
{
	Safe_AddRef(m_pNavigationCom);
	XMStoreFloat3(&m_vDirection, vDir);
}

void CPajama_BeatenState::Start(_bool IsBlend)
{
	if (TEXT("CustomMan_Beaten_Left") != m_pPajama->Get_CurrentAnim())
		m_pPajama->Set_AnimIndex("CustomMan_Beaten_Left", 1.5f, false);
	else
		m_pPajama->Set_AnimIndex("CustomMan_Beaten_Right", 1.5f, false);

	m_pPajama->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
}

CPajamaState* CPajama_BeatenState::Update(_float fTimeDelta)
{
	CPajamaState* pNextState = { nullptr };

	_bool IsAnimFinished = m_pPajama->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pPajama->Get_AnimProgress();

	/* 임시 코드. 방향 받아와서 처리해야 한다.*/
	if (false == IsAnimFinished && fAnimProgress <= 0.7f)
		m_pPajama->Get_Transform()->Go_Direction(XMVectorSetY(XMLoadFloat3(&m_vDirection), 0.f), m_fRatio * fTimeDelta * m_pGameInstance->Calc_Quadratic(-0.89f, 0.48f, 0.11f, fAnimProgress),
			m_pNavigationCom);

	if (true == IsAnimFinished)
	{
		pNextState = CPajama_IdleState::Create(m_pNavigationCom, m_pPajama);
	}

	return pNextState;
}

_bool CPajama_BeatenState::End()
{
	return true;
}

CPajama_BeatenState* CPajama_BeatenState::Create(CNavigation* pNavigation, CPajama* pPajama, _vector vDir, _float fRatio)
{
	return new CPajama_BeatenState(pNavigation, pPajama, vDir, fRatio);
}

void CPajama_BeatenState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
