#include "WhiteJetsu_IdleState.h"

#include "WhiteJetsu.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "WhiteJetsu_AttackState.h"
#include "WhiteJetsu_RunState.h"

#pragma endregion

CWhiteJetsu_IdleState::CWhiteJetsu_IdleState(class CTransform* pTransform, class CModel* pModelCom)
	: m_pTransformCom { pTransform }
	, m_pModelCom { pModelCom }
{
	Safe_AddRef(m_pTransformCom);
	Safe_AddRef(m_pModelCom);
}

void CWhiteJetsu_IdleState::Start(_bool IsBlend)
{
	m_pModelCom->Set_AnimIndex("WhiteZetsuCrowdForm_Idle_Type02_Loop", 1.f, true);
}

CWhiteJetsuState* CWhiteJetsu_IdleState::Update(_float fTimeDelta)
{
	CWhiteJetsuState* pNextState = { nullptr };

	m_pModelCom->Play_Animation(fTimeDelta);

	return pNextState;
}

_bool CWhiteJetsu_IdleState::End()
{
	return false;
}


CWhiteJetsu_IdleState* CWhiteJetsu_IdleState::Create(CTransform* pTransform, CModel* pModelCom)
{
	return new CWhiteJetsu_IdleState(pTransform, pModelCom);
}

void CWhiteJetsu_IdleState::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
}
