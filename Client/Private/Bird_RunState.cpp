#include "Bird_RunState.h"

#include "Bird.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Bird_IdleState.h"
#include "Bird_AttackState.h"

#pragma endregion

CBird_RunState::CBird_RunState(CTransform* pTransform, CNavigation* pNavigation, CModel* pModelCom)
    : m_pTransformCom{ pTransform }
    , m_pNavigationCom{ pNavigation }
    , m_pModelCom{ pModelCom }
{
    Safe_AddRef(m_pTransformCom);
    Safe_AddRef(m_pNavigationCom);
    Safe_AddRef(m_pModelCom);
}

void CBird_RunState::Start(_bool IsBlend)
{
    m_pModelCom->Set_AnimIndex("TenTailsCloneLoser02_Run_Loop", 1.f, IsBlend);
    m_pPlayerTransformCom = CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform();
    Safe_AddRef(m_pPlayerTransformCom);
}

CBirdState* CBird_RunState::Update(_float fTimeDelta)
{
    CBirdState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pModelCom->Play_Animation(fTimeDelta);

    if (10.f >= XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION))))
    {
        pNextState = CBird_AttackState::Create(m_pTransformCom, m_pNavigationCom, m_pModelCom);
    }
    else
    {
        m_pTransformCom->Chase_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION), fTimeDelta, m_pNavigationCom, 1.f);
        m_pTransformCom->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
    }

    return pNextState;
}

_bool CBird_RunState::End()
{
    return true;
}

CBird_RunState* CBird_RunState::Create(CTransform* pTransform, CNavigation* pNavigation, CModel* pModelCom)
{
    return new CBird_RunState(pTransform, pNavigation, pModelCom);
}

void CBird_RunState::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pPlayerTransformCom);
}
