#include "Bird_DeadState.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Bird.h"

CBird_DeadState::CBird_DeadState(CNavigation* pNavigation, CBird* pBird)
	: m_pNavigationCom { pNavigation }
	, m_pBird { pBird }
	, m_pGameManager { CGameManager::GetInstance() }
{
	Safe_AddRef(m_pNavigationCom);
	Safe_AddRef(m_pGameManager);
}

void CBird_DeadState::Start(_bool IsBlend)
{
	m_pGameManager->Active_KO();
	m_pBird->Set_AnimIndex("TenTailsCloneLoser02_Dying_Type01", 1.5f, true);
}

CBirdState* CBird_DeadState::Update(_float fTimeDelta)
{
	_bool IsAnimFinished = m_pBird->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pBird->Get_AnimProgress();

	if (true == IsAnimFinished)
	{
		m_pBird->Fade_Particle();
		m_pBird->Set_Dead(true);
	}

	return nullptr;
}

_bool CBird_DeadState::End()
{
	return true;;
}

CBird_DeadState* CBird_DeadState::Create(CNavigation* pNavigation, CBird* pBird)
{
	return new CBird_DeadState(pNavigation, pBird);
}

void CBird_DeadState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pGameManager);
}
