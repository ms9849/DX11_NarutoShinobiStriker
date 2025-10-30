#include "Boss_DeadState.h"

#include "GameManager.h"
#include "GameInstance.h"
#include "Boss.h"

#include "ThousandArm.h"
/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#pragma endregion

CBoss_DeadState::CBoss_DeadState(CNavigation* pNavigation, CBoss* pBoss)
	: m_pBoss { pBoss }
	, m_pNavigationCom { pNavigation }
{
	Safe_AddRef(m_pNavigationCom);
}

void CBoss_DeadState::Start(_bool IsBlend)
{
	m_pGameManager->Active_KO();
	m_pBoss->Set_AnimIndex("CustomMan_Dying_Type01", 1.5f, true);

	static_cast<CThousandArm*>(m_pGameInstance->Get_GameObject(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Layer_ThousandArm"), 0))->Set_Visible(false);
	static_cast<CThousandArm*>(m_pGameInstance->Get_GameObject(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Layer_ThousandArm"), 0))->Set_Visible(false);

}

CBossState* CBoss_DeadState::Update(_float fTimeDelta)
{
	CBossState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pBoss->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pBoss->Get_AnimProgress();

	if (true == IsAnimFinished)
	{
		m_pBoss->Fade_Particle();
		m_pBoss->Set_Dead(true);
	}

	return nullptr;
}

_bool CBoss_DeadState::End()
{
	return false;
}

CBoss_DeadState* CBoss_DeadState::Create(CNavigation* pNavigation, CBoss* pBoss)
{
	return new CBoss_DeadState(pNavigation, pBoss);
}

void CBoss_DeadState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
