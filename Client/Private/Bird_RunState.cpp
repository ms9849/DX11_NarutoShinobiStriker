#include "Bird_RunState.h"

#include "Bird.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Bird_IdleState.h"
#include "Bird_AttackState.h"
#include "Bird_WalkState.h"
  
#pragma endregion

CBird_RunState::CBird_RunState(CNavigation* pNavigation, CBird* pBird)
    : m_pNavigationCom{ pNavigation }
    , m_pBird { pBird }
{
    Safe_AddRef(m_pNavigationCom);
}

void CBird_RunState::Start(_bool IsBlend)
{
    m_pBird->Set_AnimIndex("TenTailsCloneLoser02_Run_Loop", 1.f, IsBlend);
    m_pPlayerTransformCom = CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform();
    Safe_AddRef(m_pPlayerTransformCom);
}

CBirdState* CBird_RunState::Update(_float fTimeDelta)
{
    CBirdState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pBird->Play_Animation(fTimeDelta);
    _float fDist = XMVectorGetX(XMVector3Length(m_pBird->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));

    if (fDist <= 10.f && true == m_pBird->Use_Skill())
    {
        pNextState = CBird_AttackState::Create(m_pNavigationCom, m_pBird);
    }
    else if (fDist <= 10.f && false == m_pBird->Use_Skill())
    {
        pNextState = CBird_WalkState::Create(m_pNavigationCom, m_pBird);
    }
    else
    {
        m_pBird->Get_Transform()->Chase_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION), fTimeDelta, m_pNavigationCom, 3.f);
        m_pBird->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
    }

    return pNextState;
}

_bool CBird_RunState::End()
{
    return true;
}

CBird_RunState* CBird_RunState::Create(CNavigation* pNavigation, CBird* pBird)
{
    return new CBird_RunState(pNavigation, pBird);
}

void CBird_RunState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pPlayerTransformCom);
}
