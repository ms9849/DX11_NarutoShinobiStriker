#include "Boxer_RunState.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Boxer.h"

#pragma region TRANSFER_STATE

#include "Boxer_AttackState.h"
#include "Boxer_IdleState.h"
#include "Boxer_LeafHurricaneState.h"

#pragma endregion	

CBoxer_RunState::CBoxer_RunState(CNavigation* pNavigation, CBoxer* pBoxer)
	: m_pNavigationCom{ pNavigation }
	, m_pBoxer{ pBoxer }
{
	Safe_AddRef(m_pNavigationCom);
}


void CBoxer_RunState::Start(_bool IsBlend)
{
	m_pBoxer->Set_AnimIndex("CustomMan_Run_Loop", 1.f, IsBlend, 0.1f, true);
}

CBoxerState* CBoxer_RunState::Update(_float fTimeDelta)
{
	CBoxerState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pBoxer->Play_Animation(fTimeDelta);

	m_pBoxer->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
	m_pBoxer->Get_Transform()->Chase_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION), fTimeDelta);

	_float fDist = XMVectorGetX(XMVector3Length(m_pBoxer->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));

	/* ³ª¹µÀÙ ¼±Ç³ »ç¿ë */
	if (fDist < 10.f && true == m_pBoxer->Use_Skill())
		pNextState = CBoxer_LeafHurricaneState::Create(m_pNavigationCom, m_pBoxer);

	else if (fDist < 2.f)
		pNextState = CBoxer_AttackState::Create(m_pNavigationCom, m_pBoxer);

	return pNextState;
}

_bool CBoxer_RunState::End()
{
	return true;
}

CBoxer_RunState* CBoxer_RunState::Create(CNavigation* pNavigation, CBoxer* pBoxer)
{
	return new CBoxer_RunState(pNavigation, pBoxer);
}

void CBoxer_RunState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
