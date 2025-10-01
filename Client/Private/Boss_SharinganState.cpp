#include "Boss_SharinganState.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Boss.h"

#pragma region TRANSFER_STATE

#include "Boss_IdleState.h"

#pragma endregion	

CBoss_SharinganState::CBoss_SharinganState(CNavigation* pNavigation, CBoss* pBoss)
	: m_pNavigationCom { pNavigation}
	, m_pBoss { pBoss }
{
	Safe_AddRef(m_pNavigationCom);
}

void CBoss_SharinganState::Start(_bool IsBlend)
{
	m_pBoss->Set_AnimIndex("CustomMan_Bee_HandSeal_RecoveryChakra_Start", 1.0f, true);
}

CBossState* CBoss_SharinganState::Update(_float fTimeDelta)
{
	CBossState* pNextState = { nullptr };
	_float fAnimProgress = m_pBoss->Get_AnimProgress();
	_bool IsAnimFinished = m_pBoss->Play_Animation(fTimeDelta);

	if (true == IsAnimFinished)
	{
		pNextState = CBoss_IdleState::Create(m_pNavigationCom, m_pBoss);
	}
	
	return pNextState;
}

_bool CBoss_SharinganState::End()
{
	return true;
}

CBoss_SharinganState* CBoss_SharinganState::Create(CNavigation* pNavigation, CBoss* pBoss)
{
	return new CBoss_SharinganState(pNavigation, pBoss);
}

void CBoss_SharinganState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
