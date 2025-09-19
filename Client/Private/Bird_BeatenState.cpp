#include "Bird_BeatenState.h"

#include "Bird.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Bird_IdleState.h"

#pragma endregion

CBird_BeatenState::CBird_BeatenState(CNavigation* pNavigation, CBird* pBird, _vector vDirection, _float fRatio)
	: m_pBird { pBird }
	, m_pNavigationCom{ pNavigation }
	, m_fRatio{ fRatio }
{
	Safe_AddRef(m_pNavigationCom);
	XMStoreFloat3(&m_vDirection, vDirection);
	m_vDirection.y = 0.f;
}

void CBird_BeatenState::Start(_bool IsBlend)
{
	if (TEXT("TenTailsCloneLoser02_Beaten_Left") != m_pBird->Get_CurrentAnim())
		m_pBird->Set_AnimIndex("TenTailsCloneLoser02_Beaten_Left", 1.5f, IsBlend);
	else
		m_pBird->Set_AnimIndex("TenTailsCloneLoser02_Beaten_Right", 1.5f, IsBlend);

	m_pPlayerTransformCom = CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform();
	Safe_AddRef(m_pPlayerTransformCom);

	/* 맞을때 플레이어 바라보게 */
	m_pBird->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
}

CBirdState* CBird_BeatenState::Update(_float fTimeDelta)
{
	CBirdState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pBird->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pBird->Get_AnimProgress();

	/* 임시 코드. 방향 받아와서 처리해야 한다.*/
	if (false == IsAnimFinished && fAnimProgress <= 0.7f)
		m_pBird->Get_Transform()->Go_Direction(XMLoadFloat3(&m_vDirection), m_fRatio * fTimeDelta * m_pGameInstance->Calc_Quadratic(-0.89f, 0.48f, 0.11f, fAnimProgress),
			m_pNavigationCom);

	if (true == IsAnimFinished)
	{
		pNextState = CBird_IdleState::Create(m_pNavigationCom, m_pBird);
	}

	return pNextState;
}

_bool CBird_BeatenState::End()
{
	return true;
}

CBird_BeatenState* CBird_BeatenState::Create(CNavigation* pNavigation, CBird* pBird, _vector vDirection, _float fRatio)
{
	return new CBird_BeatenState(pNavigation, pBird, vDirection, fRatio);
}

void CBird_BeatenState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pPlayerTransformCom);
}
