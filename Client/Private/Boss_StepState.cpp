#include "Boss_StepState.h"

#include "Boss.h"
#include "GameInstance.h"
#pragma region TRANSFER_STATE

#include "Boss_IdleState.h"

#pragma endregion

CBoss_StepState::CBoss_StepState(CNavigation* pNavigationCom, CBoss* pBoss, ANIM_STATE eAnimState)
	: m_pNavigationCom { pNavigationCom}
	, m_pBoss { pBoss }
	, m_eAnimState { eAnimState }
{
	Safe_AddRef(m_pNavigationCom);
}

void CBoss_StepState::Start(_bool IsBlend)
{
	if (ANIM_STATE::LEFT == m_eAnimState)
		m_pBoss->Set_AnimIndex("CustomMan_DashStep_Left", 1.4f, IsBlend);

	else if (ANIM_STATE::RIGHT == m_eAnimState)
		m_pBoss->Set_AnimIndex("CustomMan_DashStep_Right", 1.4f, IsBlend);

	else if (ANIM_STATE::BACK == m_eAnimState)
		m_pBoss->Set_AnimIndex("CustomMan_DashStep_Behind", 1.4f, IsBlend);
}

CBossState* CBoss_StepState::Update(_float fTimeDelta)
{
	CBossState* pNextState = { nullptr };

	_bool IsAnimFinished = m_pBoss->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pBoss->Get_AnimProgress();

	if (true == IsAnimFinished)
		pNextState = CBoss_IdleState::Create(m_pNavigationCom, m_pBoss);
	//ratio 가져와서 일정 이상일떄만 이동
	// 왼쪽 짧대시
	else if (ANIM_STATE::LEFT == m_eAnimState)
	{
		_float fStepSpeed = m_pGameInstance->Calc_Quadratic(-5.f, 4.f, 1.f, fAnimProgress);
		if (fAnimProgress <= 0.8f)
			m_pBoss->Get_Transform()->Go_Left(fTimeDelta * fStepSpeed, m_pNavigationCom);
	}
	else if (ANIM_STATE::RIGHT == m_eAnimState)
	{
		_float fStepSpeed = m_pGameInstance->Calc_Quadratic(-5.f, 4.f, 1.f, fAnimProgress);

		if (fAnimProgress <= 0.8f)
			m_pBoss->Get_Transform()->Go_Right(fTimeDelta * fStepSpeed, m_pNavigationCom);
	}
	else if (ANIM_STATE::BACK == m_eAnimState)
	{
		_float fStepSpeed = m_pGameInstance->Calc_Quadratic(-5.f, 4.f, 1.f, fAnimProgress);

		if (fAnimProgress != 0.f && fAnimProgress <= 0.6f)
			m_pBoss->Get_Transform()->Go_Backward(fTimeDelta * fStepSpeed, m_pNavigationCom);
	}

	return pNextState;
}

_bool CBoss_StepState::End()
{
	return true;
}

CBoss_StepState* CBoss_StepState::Create(CNavigation* pNavigationCom, CBoss* pBoss, ANIM_STATE eAnimState)
{
	return new CBoss_StepState(pNavigationCom, pBoss, eAnimState);
}

void CBoss_StepState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
