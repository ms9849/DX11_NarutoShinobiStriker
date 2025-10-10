#include "Boxer_StepState.h"

#include "Boxer.h"
#include "GameInstance.h"
#pragma region TRANSFER_STATE

#include "Boxer_IdleState.h"

#pragma endregion

CBoxer_StepState::CBoxer_StepState(CNavigation* pNavigationCom, CBoxer* pBoxer, ANIM_STATE eAnimState)
	: m_pBoxer { pBoxer }
	, m_eAnimState { eAnimState }
	, m_pNavigationCom { pNavigationCom }
{
	Safe_AddRef(m_pNavigationCom);
}

void CBoxer_StepState::Start(_bool IsBlend)
{
	if (ANIM_STATE::LEFT == m_eAnimState)
		m_pBoxer->Set_AnimIndex("CustomMan_DashStep_Left", 1.4f, IsBlend);

	else if (ANIM_STATE::RIGHT == m_eAnimState)
		m_pBoxer->Set_AnimIndex("CustomMan_DashStep_Right", 1.4f, IsBlend);

	else if (ANIM_STATE::BACK == m_eAnimState)
		m_pBoxer->Set_AnimIndex("CustomMan_DashStep_Behind", 1.4f, IsBlend);
}

CBoxerState* CBoxer_StepState::Update(_float fTimeDelta)
{
	CBoxerState* pNextState = { nullptr };

	_bool IsAnimFinished = m_pBoxer->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pBoxer->Get_AnimProgress();

	if (true == IsAnimFinished)
		pNextState = CBoxer_IdleState::Create(m_pNavigationCom, m_pBoxer);
	//ratio 가져와서 일정 이상일떄만 이동
	// 왼쪽 짧대시
	else if (ANIM_STATE::LEFT == m_eAnimState)
	{
		_float fStepSpeed = m_pGameInstance->Calc_Quadratic(-5.f, 4.f, 1.f, fAnimProgress);
		if (fAnimProgress <= 0.8f)
			m_pBoxer->Get_Transform()->Go_Left(fTimeDelta * fStepSpeed, m_pNavigationCom);
	}
	else if (ANIM_STATE::RIGHT == m_eAnimState)
	{
		_float fStepSpeed = m_pGameInstance->Calc_Quadratic(-5.f, 4.f, 1.f, fAnimProgress);

		if (fAnimProgress <= 0.8f)
			m_pBoxer->Get_Transform()->Go_Right(fTimeDelta * fStepSpeed, m_pNavigationCom);
	}
	else if (ANIM_STATE::BACK == m_eAnimState)
	{
		_float fStepSpeed = m_pGameInstance->Calc_Quadratic(-5.f, 4.f, 1.f, fAnimProgress);

		if (fAnimProgress != 0.f && fAnimProgress <= 0.6f)
			m_pBoxer->Get_Transform()->Go_Backward(fTimeDelta * fStepSpeed, m_pNavigationCom);
	}

	return pNextState;
}

_bool CBoxer_StepState::End()
{
	return true;
}

CBoxer_StepState* CBoxer_StepState::Create(CNavigation* pNavigationCom, CBoxer* pBoxer, ANIM_STATE eAnimState)
{
	return new CBoxer_StepState(pNavigationCom, pBoxer, eAnimState);
}

void CBoxer_StepState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
