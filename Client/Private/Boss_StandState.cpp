#include "Boss_StandState.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Boss.h"

#pragma region TRANSFER_STATE

#include "Boss_IdleState.h"


#pragma endregion

CBoss_StandState::CBoss_StandState(CNavigation* pNavigation, CBoss* pBoss)
	: m_pNavigationCom{ pNavigation }
	, m_pBoss{ pBoss }
{
	Safe_AddRef(m_pNavigationCom);
}

void CBoss_StandState::Start(_bool IsBlend)
{
	m_pBoss->Set_AnimIndex("CustomMan_etc_Emotion_etc_Win_Type01_Loop", 1.f, IsBlend, 0.1f);
}

CBossState* CBoss_StandState::Update(_float fTimeDelta)
{
	_bool IsAnimFinished = m_pBoss->Play_Animation(fTimeDelta);

	return nullptr;
}

_bool CBoss_StandState::End()
{
	return true;
}

CBoss_StandState* CBoss_StandState::Create(CNavigation* pNavigation, CBoss* pBoss)
{
	return new CBoss_StandState(pNavigation, pBoss);
}

void CBoss_StandState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
