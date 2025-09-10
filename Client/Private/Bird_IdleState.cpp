#include "Bird_IdleState.h"

#include "Bird.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Bird_AttackState.h"
#include "Bird_RunState.h"

#pragma endregion

CBird_IdleState::CBird_IdleState(CTransform* pTransform, CNavigation* pNavigation, CModel* pModelCom)
    : m_pTransformCom { pTransform }
    , m_pNavigationCom { pNavigation }
    , m_pModelCom { pModelCom}
{
    Safe_AddRef(m_pTransformCom);
    Safe_AddRef(m_pNavigationCom);
    Safe_AddRef(m_pModelCom);
}

void CBird_IdleState::Start(_bool IsBlend)
{
    m_pModelCom->Set_AnimIndex("TenTailsCloneLoser02_Idle_Type02_Loop", 1.f, IsBlend);
    m_pPlayerTransformCom = CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform();
    Safe_AddRef(m_pPlayerTransformCom);
}

CBirdState* CBird_IdleState::Update(_float fTimeDelta)
{
    CBirdState* pNextState = { nullptr };
    m_pModelCom->Play_Animation(fTimeDelta);

    _float fDist = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));
    m_fTimeAcc += fTimeDelta;

    /* 새는 원거리 공격 가능하니까. */
    if (fDist < 10.f && 1.5f <= m_fTimeAcc)
        pNextState = CBird_AttackState::Create(m_pTransformCom, m_pNavigationCom, m_pModelCom);

    else if (fDist < 15.f && 1.5f <= m_fTimeAcc)
        pNextState = CBird_RunState::Create(m_pTransformCom, m_pNavigationCom, m_pModelCom);

    return pNextState;
}

_bool CBird_IdleState::End()
{
    return true;
}

CBird_IdleState* CBird_IdleState::Create(CTransform* pTransform, CNavigation* pNavigation, CModel* pModelCom)
{
    return new CBird_IdleState(pTransform, pNavigation, pModelCom);
}

void CBird_IdleState::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pPlayerTransformCom);
}
