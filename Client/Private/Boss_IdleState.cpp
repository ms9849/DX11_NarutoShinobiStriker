#include "Boss_IdleState.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Boss.h"

#pragma region TRANSFER_STATE

#include "Boss_RunState.h"
#include "Boss_WalkState.h"
#include "Boss_LightingRushState.h"

#pragma endregion

CBoss_IdleState::CBoss_IdleState(CNavigation* pNavigation, CBoss* pBoss)
    : m_pNavigationCom { pNavigation }
    , m_pBoss { pBoss }
{
    Safe_AddRef(m_pNavigationCom);
}

void CBoss_IdleState::Start(_bool IsBlend)
{
    m_pBoss->Set_AnimIndex("CustomMan_Idle_Loop", 1.f, IsBlend, 0.1f);
}

CBossState* CBoss_IdleState::Update(_float fTimeDelta)
{
	CBossState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pBoss->Play_Animation(fTimeDelta);

	_float fDist = XMVectorGetX(XMVector3Length(m_pBoss->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));

	/* 돌진 패턴은 거리 20 이상일때만 발동한다. */
	if (true == m_pBoss->Use_Skill(CBoss::BOSS_SKILL::LIGHTING_RUSH) && fDist >= 20.f)
		pNextState = CBoss_LightingRushState::Create(m_pNavigationCom, m_pBoss);

	else if (fDist < 10.f)
		pNextState = CBoss_WalkState::Create(m_pNavigationCom, m_pBoss);

	else
		pNextState = CBoss_RunState::Create(m_pNavigationCom, m_pBoss);

	return pNextState;
}

_bool CBoss_IdleState::End()
{
	return true;
}

CBoss_IdleState* CBoss_IdleState::Create(CNavigation* pNavigation, CBoss* pPajama)
{
	return new CBoss_IdleState(pNavigation, pPajama);
}

void CBoss_IdleState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
