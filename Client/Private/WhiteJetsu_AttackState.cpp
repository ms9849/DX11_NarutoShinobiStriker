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
	_float fAnimProgress = m_pJetsu->Get_AnimProgress();

	if(false == m_isColliderOn && fAnimProgress > 0.35f)
		Update_Collider();

	if(fAnimProgress >= 0.8f)
		m_pJetsu->Set_Collider_Active(TEXT("Com_Collider_HandAttack"), false);

	if (true == IsAnimFinished)
		pNextState = CWhiteJetsu_IdleState::Create(m_pNavigationCom, m_pJetsu);

	return pNextState;
}

_bool CWhiteJetsu_AttackState::End()
{
	return true;
}

void CWhiteJetsu_AttackState::Update_Collider()
{
	m_pJetsu->Set_Collider_Active(TEXT("Com_Collider_HandAttack"), true);
	m_isColliderOn = true;
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
