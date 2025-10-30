#include "Boxer_AttackState.h"

#include "GameManager.h"
#include "GameInstance.h"
#include "Boxer.h"

#pragma region TRANSFER_STATE

#include "Boxer_RunState.h"
#include "Boxer_IdleState.h"
#include "Boxer_StepState.h"

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
	m_pGameInstance->PlaySoundOnce(TEXT("Boxer_attack1.wav"), CHANNELID::EFFECT, 0.35f);
	m_eAnimState = ANIM_STATE::ATTACK_01;
}

CBoxerState* CBoxer_AttackState::Update(_float fTimeDelta)
{
	CBoxerState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pBoxer->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pBoxer->Get_AnimProgress();
	Update_Collider(fAnimProgress);

	if (false == IsAnimFinished && fAnimProgress <= 0.4f)
		m_pBoxer->Get_Transform()->Go_Straight(fTimeDelta * m_pGameInstance->Calc_Quadratic(-10.f, 4.f, 0.f, fAnimProgress),
			m_pNavigationCom);

	if (true == IsAnimFinished && ANIM_STATE::ATTACK_01 == m_eAnimState)
	{
		m_IsOnCollider = false;
		m_pBoxer->Set_AnimIndex("CustomMan_Attack_Hand_StraightPunch", 4.5f, false, 0.1f, false);
		m_pBoxer->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
		m_pGameInstance->PlaySoundOnce(TEXT("Boxer_attack2.wav"), CHANNELID::EFFECT, 0.35f);
		m_eAnimState = ANIM_STATE::ATTACK_02;
	}
	else if (true == IsAnimFinished && ANIM_STATE::ATTACK_02 == m_eAnimState)
	{
		_float fDist = XMVectorGetX(XMVector3Length(m_pBoxer->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));

		if (fDist < 30.f)
		{
			m_pBoxer->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));

			_float fRandom = m_pGameInstance->Random_Normal();
			if(fRandom >= 0.7f)
				pNextState = CBoxer_StepState::Create(m_pNavigationCom, m_pBoxer, CBoxer_StepState::ANIM_STATE::BACK);
			else if(fRandom >= 0.4f)
				pNextState = CBoxer_StepState::Create(m_pNavigationCom, m_pBoxer, CBoxer_StepState::ANIM_STATE::RIGHT);
			else
				pNextState = CBoxer_StepState::Create(m_pNavigationCom, m_pBoxer, CBoxer_StepState::ANIM_STATE::LEFT);

		}
		else
			pNextState = CBoxer_IdleState::Create(m_pNavigationCom, m_pBoxer);
	}

	return pNextState;
}

_bool CBoxer_AttackState::End()
{
	return true;
}

void CBoxer_AttackState::Update_Collider(_float fAnimProgress)
{
	if (fAnimProgress <= 0.2f || fAnimProgress >= 0.8f)
		m_pBoxer->Get_Collider(TEXT("Com_Collider_HandAttack"))->Set_Active(false);

	else if (false == m_IsOnCollider)
	{
		m_pBoxer->Get_Collider(TEXT("Com_Collider_HandAttack"))->Set_Active(true);
		m_IsOnCollider = true;
	}
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
