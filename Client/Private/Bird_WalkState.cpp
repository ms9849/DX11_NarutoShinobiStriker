#include "Bird_WalkState.h"

#include "Bird.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Bird_IdleState.h"
#include "Bird_AttackState.h"

#pragma endregion

CBird_WalkState::CBird_WalkState(CNavigation* pNavigation, CBird* pBird)
    : m_pNavigationCom{ pNavigation }
    , m_pBird{ pBird }
{
    Safe_AddRef(m_pNavigationCom);
}

void CBird_WalkState::Start(_bool IsBlend)
{
    m_pBird->Set_AnimIndex("TenTailsCloneLoser02_AimWalk_Right_Loop", 0.7f, IsBlend);
    m_pPlayerTransformCom = CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform();
    Safe_AddRef(m_pPlayerTransformCom);
}

CBirdState* CBird_WalkState::Update(_float fTimeDelta)
{
    CBirdState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pBird->Play_Animation(fTimeDelta);
    _float fDist = XMVectorGetX(XMVector3Length(m_pBird->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));
    
    if (10.f >= fDist && true == m_pBird->Use_Skill())
    {
        pNextState = CBird_AttackState::Create(m_pNavigationCom, m_pBird);
    }

    else if(fDist >= 5.f)
    {
        m_pBird->Get_Transform()->Go_Right(fTimeDelta * 0.1f, m_pNavigationCom);
        m_pBird->Get_Transform()->Chase(m_pPlayerTransformCom->Get_State(STATE::POSITION), fTimeDelta * 0.02f, nullptr, fDist);
        m_pBird->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
    }
    else if (fDist <= 3.f)
    {
        m_pBird->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
        m_pBird->Get_Transform()->Go_Right(fTimeDelta * 0.1f, m_pNavigationCom);
        m_pBird->Get_Transform()->Go_Backward(fTimeDelta * 0.05f, m_pNavigationCom);
    }

    return pNextState;
}

_bool CBird_WalkState::End()
{
    return true;
}

CBird_WalkState* CBird_WalkState::Create(CNavigation* pNavigation, CBird* pBird)
{
    return new CBird_WalkState(pNavigation, pBird);
}

void CBird_WalkState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pPlayerTransformCom);
}
