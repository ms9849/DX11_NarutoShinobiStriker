#include "WhiteJetsu_DeadState.h"

#include "WhiteJetsu.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#pragma endregion

CWhiteJetsu_DeadState::CWhiteJetsu_DeadState(CNavigation* pNavigation, CWhiteJetsu* pJetsu)
	: m_pJetsu { pJetsu } 
	, m_pNavigationCom { pNavigation }
	, m_pGameManager { CGameManager::GetInstance() }
{
	Safe_AddRef(m_pNavigationCom);
}

void CWhiteJetsu_DeadState::Start(_bool IsBlend)
{
	m_pGameManager->Active_KO();
	m_pJetsu->Set_AnimIndex("WhiteZetsuCrowdForm_Dying_Type01", 1.5f, true);
}

CWhiteJetsuState* CWhiteJetsu_DeadState::Update(_float fTimeDelta)
{
	CWhiteJetsuState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pJetsu->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pJetsu->Get_AnimProgress();

	if (true == IsAnimFinished)
		m_pJetsu->Set_Dead(true);

	return nullptr;
}

_bool CWhiteJetsu_DeadState::End()
{
	return false;
}

CWhiteJetsu_DeadState* CWhiteJetsu_DeadState::Create(CNavigation* pNavigation, CWhiteJetsu* pJetsu)
{
	return new CWhiteJetsu_DeadState(pNavigation, pJetsu);
}

void CWhiteJetsu_DeadState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pGameManager);
}
