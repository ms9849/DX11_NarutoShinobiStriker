#include "WhiteJetsu_IdleState.h"

#include "WhiteJetsu.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "WhiteJetsu_AttackState.h"
#include "WhiteJetsu_RunState.h"
#include "WhiteJetsu_WoodHandState.h"

#pragma endregion

CWhiteJetsu_IdleState::CWhiteJetsu_IdleState(CNavigation* pNavigation, CWhiteJetsu* pJetsu)
	: m_pJetsu{ pJetsu }
	, m_pNavigationCom{ pNavigation }
{
}

void CWhiteJetsu_IdleState::Start(_bool IsBlend)
{
	m_pJetsu->Set_AnimIndex("WhiteZetsuCrowdForm_Idle_Type02_Loop", 1.f, true);
	m_pPlayerTransformCom = CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform();
	Safe_AddRef(m_pPlayerTransformCom);
}

CWhiteJetsuState* CWhiteJetsu_IdleState::Update(_float fTimeDelta)
{
	CWhiteJetsuState* pNextState = { nullptr };

	m_pJetsu->Play_Animation(fTimeDelta);

	_float fDist = XMVectorGetX(XMVector3Length(m_pJetsu->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));
	
	if(fDist < 1.f)
		pNextState = CWhiteJetsu_AttackState::Create(m_pNavigationCom, m_pJetsu);

	else if(fDist <= 20.f && true == m_pJetsu->Use_Skill())
		pNextState = CWhiteJetsu_WoodHandState::Create(m_pNavigationCom, m_pJetsu);

	else if (fDist < 30.f)
		pNextState = CWhiteJetsu_RunState::Create(m_pNavigationCom, m_pJetsu);


	return pNextState;
}

_bool CWhiteJetsu_IdleState::End()
{
	return true;
}

CWhiteJetsu_IdleState* CWhiteJetsu_IdleState::Create(CNavigation* pNavigation, CWhiteJetsu* pJetsu)
{
	return new CWhiteJetsu_IdleState(pNavigation, pJetsu);
}

void CWhiteJetsu_IdleState::Free()
{
	__super::Free();

	Safe_Release(m_pPlayerTransformCom);
	Safe_Release(m_pNavigationCom);
}
