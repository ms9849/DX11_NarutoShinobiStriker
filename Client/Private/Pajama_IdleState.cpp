#include "Pajama_IdleState.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Player.h"
#include "Pajama.h"

#pragma region TRANSFER_STATE

#include "Pajama_RunState.h"
#include "Pajama_WalkState.h"

#pragma endregion

CPajama_IdleState::CPajama_IdleState(CNavigation* pNavigation, CPajama* pPajama)
	: m_pNavigationCom{ pNavigation }
	, m_pPajama{ pPajama }
{
	Safe_AddRef(m_pNavigationCom);
}

void CPajama_IdleState::Start(_bool IsBlend)
{
	m_pPajama->Set_AnimIndex("CustomMan_Idle_Loop", 1.f, IsBlend, 0.1f);
}

CPajamaState* CPajama_IdleState::Update(_float fTimeDelta)
{
	CPajamaState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pPajama->Play_Animation(fTimeDelta);

	_float fDist = XMVectorGetX(XMVector3Length(m_pPajama->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));

	if (fDist < 4.f)
		pNextState = CPajama_WalkState::Create(m_pNavigationCom, m_pPajama);

	else if (fDist < 15.f)
		pNextState = CPajama_RunState::Create(m_pNavigationCom, m_pPajama);

	return pNextState;
}

_bool CPajama_IdleState::End()
{
	return true;
}

CPajama_IdleState* CPajama_IdleState::Create(CNavigation* pNavigation, CPajama* pPajama)
{
	return new CPajama_IdleState(pNavigation, pPajama);
}

void CPajama_IdleState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
