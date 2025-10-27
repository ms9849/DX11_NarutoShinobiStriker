#include "Boxer_DeadState.h"

#include "GameManager.h"
#include "GameInstance.h"
#include "Boxer.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#pragma endregion

CBoxer_DeadState::CBoxer_DeadState(CNavigation* pNavigation, CBoxer* pBoxer)
	: m_pBoxer{ pBoxer }
	, m_pNavigationCom{ pNavigation }
{
	Safe_AddRef(m_pNavigationCom);
}

void CBoxer_DeadState::Start(_bool IsBlend)
{
	m_pGameManager->Active_KO();
	m_pBoxer->Set_AnimIndex("CustomMan_Dying_Type01", 1.5f, true);
}

CBoxerState* CBoxer_DeadState::Update(_float fTimeDelta)
{
	CBoxerState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pBoxer->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pBoxer->Get_AnimProgress();

	if (true == IsAnimFinished)
	{
		m_pBoxer->Fade_Particle();
		m_pBoxer->Set_Dead(true);
	}

	return nullptr;
}

_bool CBoxer_DeadState::End()
{
	return false;
}

CBoxer_DeadState* CBoxer_DeadState::Create(CNavigation* pNavigation, CBoxer* pBoxer)
{
	return new CBoxer_DeadState(pNavigation, pBoxer);
}

void CBoxer_DeadState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
