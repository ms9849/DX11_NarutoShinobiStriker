#include "WhiteJetsu_BeatenState.h"

#include "WhiteJetsu.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "WhiteJetsu_IdleState.h"

#pragma endregion

CWhiteJetsu_BeatenState::CWhiteJetsu_BeatenState(CNavigation* pNavigation, CWhiteJetsu* pJetsu)
	: m_pJetsu{ pJetsu }
	, m_pNavigationCom{ pNavigation }
{
	Safe_AddRef(m_pNavigationCom);
}

void CWhiteJetsu_BeatenState::Start(_bool IsBlend)
{
	/* 현재 구조에서 같은 애니메이션을 새로 재생하는 것은 불가능. (같은 애니끼리 재생하면 보간 + 등의 문제로)*/

	if(TEXT("WhiteZetsuCrowdForm_Beaten_Left") != m_pJetsu->Get_CurrentAnim())
		m_pJetsu->Set_AnimIndex("WhiteZetsuCrowdForm_Beaten_Left", 1.5f, true);
	else
		m_pJetsu->Set_AnimIndex("WhiteZetsuCrowdForm_Beaten_Right", 1.5f, true);

	m_pPlayerTransformCom = CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform();
	Safe_AddRef(m_pPlayerTransformCom);
}

CWhiteJetsuState* CWhiteJetsu_BeatenState::Update(_float fTimeDelta)
{
	CWhiteJetsuState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pJetsu->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pJetsu->Get_AnimProgress();

	/* 임시 코드. 방향 받아와서 처리해야 한다.*/
	if (false == IsAnimFinished && fAnimProgress <= 0.7f)
		m_pJetsu->Get_Transform()->Go_Backward(fTimeDelta * m_pGameInstance->Calc_Quadratic(-0.89f, 0.48f, 0.11f, fAnimProgress),
			m_pNavigationCom);

	if (true == IsAnimFinished)
	{
		pNextState = CWhiteJetsu_IdleState::Create(m_pNavigationCom, m_pJetsu);
	}

	return pNextState;
}

_bool CWhiteJetsu_BeatenState::End()
{
	return true;
}

CWhiteJetsu_BeatenState* CWhiteJetsu_BeatenState::Create(CNavigation* pNavigation, CWhiteJetsu* pJetsu)
{
	return new CWhiteJetsu_BeatenState(pNavigation, pJetsu);
}

void CWhiteJetsu_BeatenState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pPlayerTransformCom);
}
