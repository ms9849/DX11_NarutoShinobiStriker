#include "WhiteJetsu_AttackState.h"

#include "WhiteJetsu.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"
/* 전이 가능한 상태들 */

#pragma region TRANSFER_STATE

#include "WhiteJetsu_IdleState.h"

#pragma endregion

CWhiteJetsu_AttackState::CWhiteJetsu_AttackState(class CTransform* pTransform, class CNavigation* pNavigationCom, class CModel* pModelCom)
	: m_pTransformCom { pTransform }
	, m_pModelCom { pModelCom }
	, m_pNavigationCom { pNavigationCom }
{
	Safe_AddRef(m_pTransformCom);
	Safe_AddRef(m_pModelCom);
	Safe_AddRef(m_pNavigationCom);
}

void CWhiteJetsu_AttackState::Start(_bool IsBlend)
{
	m_pModelCom->Set_AnimIndex("WhiteZetsuCrowdForm_Attack_Punch", 1.f, true);
	m_pTransformCom->LookAt_XZ(CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform()->Get_State(STATE::POSITION));
}

CWhiteJetsuState* CWhiteJetsu_AttackState::Update(_float fTimeDelta)
{
	CWhiteJetsuState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pModelCom->Play_Animation(fTimeDelta);

	if (true == IsAnimFinished)
		pNextState = CWhiteJetsu_IdleState::Create(m_pTransformCom, m_pNavigationCom, m_pModelCom);

	return pNextState;
}

_bool CWhiteJetsu_AttackState::End()
{
	return true;
}

CWhiteJetsu_AttackState* CWhiteJetsu_AttackState::Create(class CTransform* pTransform, class CNavigation* pNavigation, class CModel* pModelCom)
{
	return new CWhiteJetsu_AttackState(pTransform, pNavigation, pModelCom);
}

void CWhiteJetsu_AttackState::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNavigationCom);
}
