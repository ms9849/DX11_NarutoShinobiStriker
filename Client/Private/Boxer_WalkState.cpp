#include "Boxer_WalkState.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Player.h"
#include "Boxer.h"

#pragma region TRANSFER_STATE

#include "Boxer_IdleState.h"
#include "Boxer_StepState.h"
#include "Boxer_RunState.h"
#pragma endregion


CBoxer_WalkState::CBoxer_WalkState(CNavigation* pNavigation, CBoxer* pBoxer)
	: m_pNavigationCom{ pNavigation }
	, m_pBoxer { pBoxer }
{
	Safe_AddRef(m_pNavigationCom);
}

void CBoxer_WalkState::Start(_bool IsBlend)
{
	m_pBoxer->Set_AnimIndex("CustomMan_Walk_Loop", 0.9f, IsBlend, 0.1f);
}

CBoxerState* CBoxer_WalkState::Update(_float fTimeDelta)
{
    CBoxerState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pBoxer->Play_Animation(fTimeDelta);
    _float fDist = XMVectorGetX(XMVector3Length(m_pBoxer->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));

	_float fRand = m_pGameInstance->Random_Normal();

	if (fRand >= 0.5f)
		pNextState = CBoxer_StepState::Create(m_pNavigationCom, m_pBoxer, CBoxer_StepState::ANIM_STATE::BACK);

	else 
		pNextState = CBoxer_IdleState::Create(m_pNavigationCom, m_pBoxer);

    return pNextState;
}

_bool CBoxer_WalkState::End()
{
	return true;
}

CBoxer_WalkState* CBoxer_WalkState::Create(CNavigation* pNavigation, CBoxer* pBoxer)
{
	return new CBoxer_WalkState(pNavigation, pBoxer);
}

void CBoxer_WalkState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
