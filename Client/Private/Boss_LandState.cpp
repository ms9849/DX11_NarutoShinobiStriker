#include "Boss_LandState.h"

#include "GameInstance.h"
#include "Boss.h"

#pragma region TRANSFRE_STATE

#include "Boss_IdleState.h"

#pragma endregion

CBoss_LandState::CBoss_LandState(CNavigation* pNavigation, CBoss* pBoss)
    : m_pNavigationCom{ pNavigation }
    , m_pBoss{ pBoss }
{
    Safe_AddRef(m_pNavigationCom);
}

void CBoss_LandState::Start(_bool IsBlend)
{
	m_pBoss->Set_AnimIndex("CustomMan_Land", 1.f, IsBlend, 0.4f);
}

CBossState* CBoss_LandState::Update(_float fTimeDelta)
{
	CBossState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pBoss->Play_Animation(fTimeDelta);

	if (true == IsAnimFinished)
	{
		pNextState = CBoss_IdleState::Create(m_pNavigationCom, m_pBoss);
		m_IsNextAnimBlened = false;
	}

	return pNextState;
}

_bool CBoss_LandState::End()
{
	return true;
}

CBoss_LandState* CBoss_LandState::Create(CNavigation* pNavigation, CBoss* pBoss)
{
	return new CBoss_LandState(pNavigation, pBoss);
}

void CBoss_LandState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
