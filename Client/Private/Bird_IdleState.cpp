#include "Bird_IdleState.h"

#include "Bird.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Bird_AttackState.h"
#include "Bird_RunState.h"
#include "Bird_WalkState.h"

#pragma endregion

CBird_IdleState::CBird_IdleState(class CNavigation* pNavigation, class CBird* pBird)
    : m_pNavigationCom { pNavigation }
    , m_pBird { pBird }
{
    Safe_AddRef(m_pNavigationCom);
}

void CBird_IdleState::Start(_bool IsBlend)
{
    m_pBird->Set_AnimIndex("TenTailsCloneLoser02_Idle_Type02_Loop", 1.f, IsBlend);
    m_pPlayerTransformCom = CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform();

    _float fDist = XMVectorGetX(XMVector3Length(m_pBird->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));
    /* 플레이어 쳐다보게 */
    if (fDist < 25.f)
        m_pBird->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));

    Safe_AddRef(m_pPlayerTransformCom);
}

CBirdState* CBird_IdleState::Update(_float fTimeDelta)
{
    CBirdState* pNextState = { nullptr };
    m_pBird->Play_Animation(fTimeDelta);

    _float fDist = XMVectorGetX(XMVector3Length(m_pBird->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));

    /* 새는 원거리 공격 가능하니까. */
    if (fDist <= 10.f && true == m_pBird->Use_Skill())
        pNextState = CBird_AttackState::Create(m_pNavigationCom, m_pBird);

    else if (fDist <= 10.f)
        pNextState = CBird_WalkState::Create(m_pNavigationCom, m_pBird);

    else if (fDist <= 20.f)
        pNextState = CBird_RunState::Create(m_pNavigationCom, m_pBird);

    return pNextState;
}

_bool CBird_IdleState::End()
{
    return true;
}

CBird_IdleState* CBird_IdleState::Create(class CNavigation* pNavigation, class CBird* pBird)
{
    return new CBird_IdleState(pNavigation, pBird);
}

void CBird_IdleState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pPlayerTransformCom);
}
