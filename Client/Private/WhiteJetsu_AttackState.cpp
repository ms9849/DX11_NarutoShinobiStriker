#include "WhiteJetsu_AttackState.h"

#include "WhiteJetsu.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"
/* 전이 가능한 상태들 */

#pragma region TRANSFER_STATE

#include "WhiteJetsu_IdleState.h"

#pragma endregion

CWhiteJetsu_AttackState::CWhiteJetsu_AttackState(class CNavigation* pNavigationCom, class CWhiteJetsu* pJetsu)
	: m_pJetsu { pJetsu }
	, m_pNavigationCom { pNavigationCom }
{
	Safe_AddRef(m_pNavigationCom);
}

void CWhiteJetsu_AttackState::Start(_bool IsBlend)
{
	m_pJetsu->Set_AnimIndex("WhiteZetsuCrowdForm_Attack_Punch", 1.f, true);
	m_pJetsu->Get_Transform()->LookAt_XZ(CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform()->Get_State(STATE::POSITION));
}

CWhiteJetsuState* CWhiteJetsu_AttackState::Update(_float fTimeDelta)
{
	CWhiteJetsuState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pJetsu->Play_Animation(fTimeDelta);

	if (true == IsAnimFinished)
		pNextState = CWhiteJetsu_IdleState::Create(m_pNavigationCom, m_pJetsu);

	return pNextState;
}

_bool CWhiteJetsu_AttackState::End()
{
	return true;
}

CWhiteJetsu_AttackState* CWhiteJetsu_AttackState::Create(class CNavigation* pNavigation, class CWhiteJetsu* pJetsu)
{
	return new CWhiteJetsu_AttackState(pNavigation, pJetsu);
}

void CWhiteJetsu_AttackState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
