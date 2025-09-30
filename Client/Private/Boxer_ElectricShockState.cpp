#include "Boxer_ElectricShockState.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Boxer.h"

#pragma region TRANSFER_STATE

#include "Boxer_IdleState.h"

#pragma endregion

CBoxer_ElectricShockState::CBoxer_ElectricShockState(CNavigation* pNavigation, CBoxer* pBoxer)
	: m_pBoxer{ pBoxer }
	, m_pNavigationCom{ pNavigation }
{
	Safe_AddRef(m_pNavigationCom);
}

void CBoxer_ElectricShockState::Start(_bool IsBlend)
{
	/* Beaten보다 훨씬 멀리 날아가야 함 */
    m_pBoxer->Set_AnimIndex("CustomMan_Beaten_ElectricShock_Loop", 2.f, true);

    m_pBoxer->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
}

CBoxerState* CBoxer_ElectricShockState::Update(_float fTimeDelta)
{
    CBoxerState* pNextState = { nullptr };
    _bool  IsAnimFinished = m_pBoxer->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pBoxer->Get_AnimProgress();

    m_fTimeAcc += fTimeDelta;

    if (m_fMaxTimeAcc <= m_fTimeAcc)
    {
        pNextState = CBoxer_IdleState::Create(m_pNavigationCom, m_pBoxer);
    }

    return pNextState;
}

_bool CBoxer_ElectricShockState::End()
{
    return true;
}

CBoxer_ElectricShockState* CBoxer_ElectricShockState::Create(CNavigation* pNavigation, CBoxer* pBoxer)
{
    return new CBoxer_ElectricShockState(pNavigation, pBoxer);
}

void CBoxer_ElectricShockState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
