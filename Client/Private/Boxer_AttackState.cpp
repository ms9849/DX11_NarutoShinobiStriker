#include "Boxer_AttackState.h"

#include "GameManager.h"
#include "GameInstance.h"
#include "Boxer.h"

#pragma region TRANSFER_STATE

#include "Boxer_RunState.h"
#include "Boxer_IdleState.h"

#pragma endregion

CBoxer_AttackState::CBoxer_AttackState(CNavigation* pNavigation, CBoxer* pBoxer)
	: m_pNavigationCom{ pNavigation }
	, m_pBoxer{ pBoxer }
{
	Safe_AddRef(m_pNavigationCom);
}

void CBoxer_AttackState::Start(_bool IsBlend)
{
	/* 공격 하면서 플레이어 바라보게 */
	m_pBoxer->Set_AnimIndex("CustomMan_Attack_Hand_Punch_Left", 2.0f, true, 0.1f, false);
	m_pBoxer->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
	m_eAnimState = ANIM_STATE::ATTACK_01;
}

CBoxerState* CBoxer_AttackState::Update(_float fTimeDelta)
{
	CBoxerState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pBoxer->Play_Animation(fTimeDelta);

	if (true == IsAnimFinished && ANIM_STATE::ATTACK_01 == m_eAnimState)
	{
		m_pBoxer->Set_AnimIndex("CustomMan_Attack_Hand_StraightPunch", 4.f, true, 0.1f, false);
		m_pBoxer->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
		m_eAnimState = ANIM_STATE::ATTACK_02;
	}
	else if (true == IsAnimFinished && ANIM_STATE::ATTACK_02 == m_eAnimState)
	{
		_float fDist = XMVectorGetX(XMVector3Length(m_pBoxer->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));

		if (fDist < 30.f)
			pNextState = CBoxer_RunState::Create(m_pNavigationCom, m_pBoxer);
		else
			pNextState = CBoxer_IdleState::Create(m_pNavigationCom, m_pBoxer);
	}


	return pNextState;
}

_bool CBoxer_AttackState::End()
{
	return true;
}

CBoxer_AttackState* CBoxer_AttackState::Create(CNavigation* pNavigation, CBoxer* pBoxer)
{
	return new CBoxer_AttackState(pNavigation, pBoxer);
}

void CBoxer_AttackState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
