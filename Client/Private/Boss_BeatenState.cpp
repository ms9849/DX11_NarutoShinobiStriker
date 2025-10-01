#include "Boss_BeatenState.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Boss.h"

#pragma region TRANSFER_STATE

#include "Boss_IdleState.h"

#pragma endregion

CBoss_BeatenState::CBoss_BeatenState(CNavigation* pNavigation, CBoss* pBoss, _vector vDir, _float fRatio)
	: m_pBoss { pBoss }
	, m_pNavigationCom{ pNavigation }
	, m_fRatio{ fRatio }
{
	Safe_AddRef(m_pNavigationCom);
	XMStoreFloat3(&m_vDirection, vDir);
}

void CBoss_BeatenState::Start(_bool IsBlend)
{
	if (TEXT("CustomMan_Beaten_Left") != m_pBoss->Get_CurrentAnim())
		m_pBoss->Set_AnimIndex("CustomMan_Beaten_Left", 1.5f, false);
	else
		m_pBoss->Set_AnimIndex("CustomMan_Beaten_Right", 1.5f, false);

	m_pBoss->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
}

CBossState* CBoss_BeatenState::Update(_float fTimeDelta)
{
	CBossState* pNextState = { nullptr };

	_bool IsAnimFinished = m_pBoss->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pBoss->Get_AnimProgress();

	/* 임시 코드. 방향 받아와서 처리해야 한다.*/
	if (false == IsAnimFinished && fAnimProgress <= 0.7f)
		m_pBoss->Get_Transform()->Go_Direction(XMVectorSetY(XMLoadFloat3(&m_vDirection), 0.f), m_fRatio * fTimeDelta * m_pGameInstance->Calc_Quadratic(-0.89f, 0.48f, 0.11f, fAnimProgress),
			m_pNavigationCom);

	if (true == IsAnimFinished)
	{
		pNextState = CBoss_IdleState::Create(m_pNavigationCom, m_pBoss);
	}

	return pNextState;
}

_bool CBoss_BeatenState::End()
{
	return true;
}

CBoss_BeatenState* CBoss_BeatenState::Create(CNavigation* pNavigation, CBoss* pBoss, _vector vDir, _float fRatio)
{
	return new CBoss_BeatenState(pNavigation, pBoss, vDir, fRatio);
}

void CBoss_BeatenState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
