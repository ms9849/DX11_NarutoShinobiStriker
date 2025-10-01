#include "Boxer_LeafHurricaneState.h"

#include "GameManager.h"
#include "GameInstance.h"
#include "Boxer.h"

#pragma region TRANSFER_STATE

#include "Boxer_RunState.h"
#include "Boxer_IdleState.h"

#pragma endregion

CBoxer_LeafHurricaneState::CBoxer_LeafHurricaneState(CNavigation* pNavigation, CBoxer* pBoxer)
	: m_pNavigationCom{ pNavigation }
	, m_pBoxer{ pBoxer }
{
	Safe_AddRef(m_pNavigationCom);
}

void CBoxer_LeafHurricaneState::Start(_bool IsBlend)
{
	m_pBoxer->Set_AnimIndex("CustomMan_Ninjutsu_LeafHurricane_Start", 1.5f, IsBlend, 0.1f, false);
	m_eAnimState = ANIM_STATE::ATTACK_START;
}

CBoxerState* CBoxer_LeafHurricaneState::Update(_float fTimeDelta)
{
	CBoxerState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pBoxer->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pBoxer->Get_AnimProgress();

	_float fDist = XMVectorGetX(XMVector3Length(m_pBoxer->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));

	Update_Collider(fAnimProgress);

	if (ANIM_STATE::ATTACK_START == m_eAnimState)
	{
		m_pBoxer->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
		m_pBoxer->Get_Transform()->Chase_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION), fTimeDelta * 2.f);
	}


	/* 나뭇잎 선풍 시작 상태에다가, 달려가는 중에 플레이어랑 가깝다면 */
	if (ANIM_STATE::ATTACK_START == m_eAnimState
		&& (fDist <= 2.0f))
	{
		m_pBoxer->Set_AnimIndex("CustomMan_Ninjutsu_LeafHurricane", 2.f, false, 0.1f, true);
		m_eAnimState = ANIM_STATE::ATTACK_END;
	}
	/* 나뭇잎 선풍 달려가는 중*/
	else if (ANIM_STATE::ATTACK_START == m_eAnimState
		&& true == IsAnimFinished)
	{
		m_pBoxer->Set_AnimIndex("CustomMan_Ninjutsu_LeafHurricane_Start", 1.f, false, 0.1f, false);
		m_pBoxer->Set_AnimProgress(1.0f);
	}
	else if (ANIM_STATE::ATTACK_END == m_eAnimState
		&& true == IsAnimFinished)
	{
		pNextState = CBoxer_IdleState::Create(m_pNavigationCom, m_pBoxer);
	}

    return pNextState;
}

_bool CBoxer_LeafHurricaneState::End()
{
	m_pBoxer->Get_Collider(TEXT("Com_Collider_LeafHurricane"))->Set_Active(false);
    return true;
}

void CBoxer_LeafHurricaneState::Update_Collider(_float fAnimProgress)
{
	if (fAnimProgress >= 0.5f)
		m_pBoxer->Get_Collider(TEXT("Com_Collider_LeafHurricane"))->Set_Active(false);

	else if (false == m_IsOnCollider && m_eAnimState == ANIM_STATE::ATTACK_END)
	{
		m_pBoxer->Get_Collider(TEXT("Com_Collider_LeafHurricane"))->Set_Active(true);
		m_IsOnCollider = true;
	}
}

CBoxer_LeafHurricaneState* CBoxer_LeafHurricaneState::Create(CNavigation* pNavigation, CBoxer* pBoxer)
{
    return new CBoxer_LeafHurricaneState(pNavigation, pBoxer);
}

void CBoxer_LeafHurricaneState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
