#include "Boss_BeatenBlastedState.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Boss.h"

#pragma region TRANSFER_STATE

#include "Boss_IdleState.h"

#pragma endregion

CBoss_BeatenBlastedState::CBoss_BeatenBlastedState(CNavigation* pNavigation, CBoss* pBoss, _vector vDir, _float fRatio)
	: m_pBoss { pBoss  }
	, m_pNavigationCom{ pNavigation }
{
	Safe_AddRef(m_pNavigationCom);
	XMStoreFloat3(&m_vDirection, vDir);
	m_vDirection.y = 0.f;
}

void CBoss_BeatenBlastedState::Start(_bool IsBlend)
{
	/* Beaten보다 훨씬 멀리 날아가야 함 */
	m_pBoss->Set_AnimIndex("CustomMan_Beaten_Blasted", 2.f, true);

    m_pBoss->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
}

CBossState* CBoss_BeatenBlastedState::Update(_float fTimeDelta)
{
    CBossState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pBoss->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pBoss->Get_AnimProgress();


    if (false == IsAnimFinished && fAnimProgress <= 0.7f)
        m_pBoss->Get_Transform()->Go_Direction(XMLoadFloat3(&m_vDirection), 1.2f * fTimeDelta * m_pGameInstance->Calc_Quadratic(-2.f, 0.f, 1.f, fAnimProgress),
            m_pNavigationCom);

    if (true == IsAnimFinished)
    {
        pNextState = CBoss_IdleState::Create(m_pNavigationCom, m_pBoss);
    }

    return pNextState;
}

_bool CBoss_BeatenBlastedState::End()
{
    return true;
}

CBoss_BeatenBlastedState* CBoss_BeatenBlastedState::Create(CNavigation* pNavigation, CBoss* pBoss, _vector vDir, _float fRatio)
{
    return new CBoss_BeatenBlastedState(pNavigation, pBoss, vDir, fRatio);
}

void CBoss_BeatenBlastedState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
