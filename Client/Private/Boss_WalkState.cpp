#include "Boss_WalkState.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Player.h"
#include "Boss.h"

#pragma region TRANSFER_STATE

//#include "Boss_HandAttackState.h"
#include "Boss_FireballState.h"
#include "Boss_RunState.h"
#include "Boss_SpinKickState.h"

#pragma endregion

CBoss_WalkState::CBoss_WalkState(CNavigation* pNavigation, CBoss* pBoss)
	: m_pNavigationCom { pNavigation }
	, m_pBoss{ pBoss }
{
	Safe_AddRef(m_pNavigationCom);
}
void CBoss_WalkState::Start(_bool IsBlend)
{
	m_pBoss->Set_AnimIndex("CustomMan_Walk_Loop", 0.9f, IsBlend, 0.1f);
}

CBossState* CBoss_WalkState::Update(_float fTimeDelta)
{
	CBossState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pBoss->Play_Animation(fTimeDelta);
	_float fDist = XMVectorGetX(XMVector3Length(m_pBoss->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));

	/* 만약 멀다면 Run, */
	/* 스킬 사용 */

	if (true == m_pBoss->Use_Skill(CBoss::BOSS_SKILL::SPIN_KICK) && fDist <= 20.f)
	{
		pNextState = CBoss_SpinKickState::Create(m_pNavigationCom, m_pBoss);
	}
	else if (true == m_pBoss->Use_Skill(CBoss::BOSS_SKILL::FIREBALL) && fDist >= 5.f)
	{
		pNextState = CBoss_FireBallState::Create(m_pNavigationCom, m_pBoss);
	}
	/* Run 으로의 전환 */
	else if (fDist >= 10.f)
	{
		pNextState = CBoss_RunState::Create(m_pNavigationCom, m_pBoss);
	}

	/* 안멀면 천천히 접근 */
	else if (fDist >= 1.3f)
	{
		m_pBoss->Get_Transform()->Go_Left(fTimeDelta * 0.1f, m_pNavigationCom);
		m_pBoss->Get_Transform()->Chase(m_pPlayerTransformCom->Get_State(STATE::POSITION), fTimeDelta * 0.05f, m_pNavigationCom, fDist);
		m_pBoss->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
	}

	/* 굳이 때리진 않고 슬라이딩 쿠나이 */
	else
	{

	}

	return pNextState;
}

_bool CBoss_WalkState::End()
{
	return true;
}

CBoss_WalkState* CBoss_WalkState::Create(CNavigation* pNavigation, CBoss* pBoss)
{
	return new CBoss_WalkState(pNavigation, pBoss);
}

void CBoss_WalkState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
