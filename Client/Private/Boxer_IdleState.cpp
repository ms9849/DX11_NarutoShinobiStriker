#include "Boxer_IdleState.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Boxer.h"

#pragma region TRANSFER_STATE

#include "Boxer_RunState.h"
#include "Boxer_AttackState.h"
#include "Boxer_LeafHurricaneState.h"
#include "Boxer_SpinKickState.h"

#pragma endregion

CBoxer_IdleState::CBoxer_IdleState(CNavigation* pNavigation, CBoxer* pBoxer)
    : m_pNavigationCom{ pNavigation }
    , m_pBoxer { pBoxer }
{
    Safe_AddRef(m_pNavigationCom);
}

void CBoxer_IdleState::Start(_bool IsBlend)
{
    m_pBoxer->Set_AnimIndex("CustomMan_Idle_Loop", 1.f, IsBlend, 0.1f);
}

CBoxerState* CBoxer_IdleState::Update(_float fTimeDelta)
{
    CBoxerState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pBoxer->Play_Animation(fTimeDelta);

    _float fDist = XMVectorGetX(XMVector3Length(m_pBoxer->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));

    /* ³ª¹µÀÙ ¼±Ç³ »ç¿ë */
    if (fDist < 15.f && true == m_pBoxer->Use_Skill())
        pNextState = CBoxer_LeafHurricaneState::Create(m_pNavigationCom, m_pBoxer);

    else if (fDist < 15.f && true == m_pBoxer->Use_SpinKick())
        pNextState = CBoxer_SpinKickState::Create(m_pNavigationCom, m_pBoxer);

    else if (fDist < 2.f)
        pNextState = CBoxer_AttackState::Create(m_pNavigationCom, m_pBoxer);

    else if (fDist < 30.f)
        pNextState = CBoxer_RunState::Create(m_pNavigationCom, m_pBoxer);

    return pNextState;
}

_bool CBoxer_IdleState::End()
{
    return true;
}

CBoxer_IdleState* CBoxer_IdleState::Create(CNavigation* pNavigation, CBoxer* pBoxer)
{
    return new CBoxer_IdleState(pNavigation, pBoxer);
}

void CBoxer_IdleState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
