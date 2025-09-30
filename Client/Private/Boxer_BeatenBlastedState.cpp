#include "Boxer_BeatenBlastedState.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Boxer.h"

#pragma region TRANSFER_STATE

#include "Boxer_IdleState.h"

#pragma endregion

CBoxer_BeatenBlastedState::CBoxer_BeatenBlastedState(CNavigation* pNavigation, CBoxer* pBoxer, _vector vDir, _float fRatio)
	: m_pBoxer { pBoxer }
	, m_pNavigationCom{ pNavigation }
{
	Safe_AddRef(m_pNavigationCom);
	XMStoreFloat3(&m_vDirection, vDir);
	m_vDirection.y = 0.f;
}

void CBoxer_BeatenBlastedState::Start(_bool IsBlend)
{
	/* Beaten보다 훨씬 멀리 날아가야 함 */
	m_pBoxer->Set_AnimIndex("CustomMan_Beaten_Blasted", 2.f, true);

    m_pBoxer->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
}

CBoxerState* CBoxer_BeatenBlastedState::Update(_float fTimeDelta)
{
    CBoxerState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pBoxer->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pBoxer->Get_AnimProgress();


    if (false == IsAnimFinished && fAnimProgress <= 0.7f)
        m_pBoxer->Get_Transform()->Go_Direction(XMLoadFloat3(&m_vDirection), 1.2f * fTimeDelta * m_pGameInstance->Calc_Quadratic(-2.f, 0.f, 1.f, fAnimProgress),
            m_pNavigationCom);

    if (true == IsAnimFinished)
    {
        pNextState = CBoxer_IdleState::Create(m_pNavigationCom, m_pBoxer);
    }

    return pNextState;
}

_bool CBoxer_BeatenBlastedState::End()
{
    return true;
}

CBoxer_BeatenBlastedState* CBoxer_BeatenBlastedState::Create(CNavigation* pNavigation, CBoxer* pBoxer, _vector vDir, _float fRatio)
{
    return new CBoxer_BeatenBlastedState(pNavigation, pBoxer, vDir, fRatio);
}

void CBoxer_BeatenBlastedState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
