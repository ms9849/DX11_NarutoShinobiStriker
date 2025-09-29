#include "Pajama_BeatenBlastedState.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Pajama.h"

#pragma region TRANSFER_STATE

#include "Pajama_IdleState.h"

#pragma endregion

CPajama_BeatenBlastedState::CPajama_BeatenBlastedState(CNavigation* pNavigation, CPajama* pPajama, _vector vDir, _float fRatio)
    : m_pPajama { pPajama }
    , m_pNavigationCom{ pNavigation }
{
    Safe_AddRef(m_pNavigationCom);
    XMStoreFloat3(&m_vDirection, vDir);
    m_vDirection.y = 0.f;
}

void CPajama_BeatenBlastedState::Start(_bool IsBlend)
{
    /* Beaten보다 훨씬 멀리 날아가야 함 */
    m_pPajama->Set_AnimIndex("CustomMan_Beaten_Blasted", 2.f, true);

    m_pPajama->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
}

CPajamaState* CPajama_BeatenBlastedState::Update(_float fTimeDelta)
{
    CPajamaState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pPajama->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPajama->Get_AnimProgress();


    if (false == IsAnimFinished && fAnimProgress <= 0.7f)
        m_pPajama->Get_Transform()->Go_Direction(XMLoadFloat3(&m_vDirection), 1.2f * fTimeDelta * m_pGameInstance->Calc_Quadratic(-2.f, 0.f, 1.f, fAnimProgress),
            m_pNavigationCom);

    if (true == IsAnimFinished)
    {
        pNextState = CPajama_IdleState::Create(m_pNavigationCom, m_pPajama);
    }

    return pNextState;
}

_bool CPajama_BeatenBlastedState::End()
{
    return true;
}

CPajama_BeatenBlastedState* CPajama_BeatenBlastedState::Create(CNavigation* pNavigation, CPajama* pPajama, _vector vDir, _float fRatio)
{
    return new CPajama_BeatenBlastedState(pNavigation, pPajama, vDir, fRatio);
}

void CPajama_BeatenBlastedState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
