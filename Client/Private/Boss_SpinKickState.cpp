#include "Boss_SpinKickState.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Boss.h"

#pragma region TRANSFER_STATE

#include "Boss_IdleState.h"
#include "Boss_StepState.h"

#pragma endregion	

CBoss_SpinKickState::CBoss_SpinKickState(CNavigation* pNavigation, CBoss* pBoss)
	: m_pBoss { pBoss}
	, m_pNavigationCom{ pNavigation }
{
	Safe_AddRef(m_pNavigationCom);
}

void CBoss_SpinKickState::Start(_bool IsBlend)
{
	m_pBoss->Set_AnimIndex("CustomMan_Ninjutsu_LeafHurricane_Start", 1.5f, IsBlend, 0.1f, false);
	m_eAnimState = ANIM_STATE::ATTACK_START;
}

CBossState* CBoss_SpinKickState::Update(_float fTimeDelta)
{
	CBossState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pBoss->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pBoss->Get_AnimProgress();
	_float fDist = XMVectorGetX(XMVector3Length(m_pBoss->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));

	Update_Collider(fAnimProgress);

	if (ANIM_STATE::ATTACK_START == m_eAnimState)
	{
		m_pBoss->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
		m_pBoss->Get_Transform()->Chase_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION), 
			fTimeDelta * 2.f, m_pNavigationCom);
	}

	/* 나뭇잎 선풍 시작 상태에다가, 달려가는 중에 플레이어랑 가깝다면 */
	if (ANIM_STATE::ATTACK_START == m_eAnimState
		&& (fDist <= 1.2f))
	{
		m_pBoss->Set_AnimIndex("CustomMan_Ninjutsu_D54NJ3_SpinkickEnd", 2.f, false, 0.1f, true);
		m_eAnimState = ANIM_STATE::ATTACK_END;
	}
	/* 나뭇잎 선풍 달려가는 중*/
	else if (ANIM_STATE::ATTACK_START == m_eAnimState
		&& true == IsAnimFinished)
	{
		m_pBoss->Set_AnimIndex("CustomMan_Ninjutsu_LeafHurricane_Start", 1.f, false, 0.1f, false);
		m_pBoss->Set_AnimProgress(1.0f);
	}
	else if (ANIM_STATE::ATTACK_END == m_eAnimState
		&& true == IsAnimFinished)
	{
		pNextState = CBoss_StepState::Create(m_pNavigationCom, m_pBoss, CBoss_StepState::ANIM_STATE::BACK);
	}

	return pNextState;
}

_bool CBoss_SpinKickState::End()
{
	return true;
}

void CBoss_SpinKickState::Update_Collider(_float fAnimProgress)
{
	//if (fAnimProgress >= 0.7f)
	//	m_pBoss->Get_Collider(TEXT("Com_Collider_SpinKick"))->Set_Active(false);

	//else if (false == m_IsOnCollider && m_eAnimState == ANIM_STATE::ATTACK_END)
	//{
	//	m_pBoss->Get_Collider(TEXT("Com_Collider_SpinKick"))->Set_Active(true);
	//	m_IsOnCollider = true;
	//}
}

CBoss_SpinKickState* CBoss_SpinKickState::Create(CNavigation* pNavigation, CBoss* pBoss)
{
	return new CBoss_SpinKickState(pNavigation, pBoss);
}

void CBoss_SpinKickState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
