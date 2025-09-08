#include "WhiteJetsu_IdleState.h"

#include "WhiteJetsu.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

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
	m_pPlayerTransformCom = CGameManager::GetInstance()->Get_PlayerPtr()->Get_PlayerTransformPtr();
	Safe_AddRef(m_pPlayerTransformCom);
}

CWhiteJetsuState* CWhiteJetsu_IdleState::Update(_float fTimeDelta)
{
	CWhiteJetsuState* pNextState = { nullptr };

	m_pModelCom->Play_Animation(fTimeDelta);

	_float fDist = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));
	
	if(fDist < 1.f)
		pNextState = CWhiteJetsu_AttackState::Create(m_pTransformCom, m_pModelCom);

	else if (fDist < 30.f)
		pNextState = CWhiteJetsu_RunState::Create(m_pTransformCom, m_pModelCom);


	return pNextState;
}

_bool CWhiteJetsu_IdleState::End()
{
	return true;
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
	Safe_Release(m_pPlayerTransformCom);
}
