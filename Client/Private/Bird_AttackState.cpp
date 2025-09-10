#include "Bird_AttackState.h"

#include "Bird.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Bird_IdleState.h"
#include "Bird_RunState.h"

#pragma endregion

CBird_AttackState::CBird_AttackState(CTransform* pTransform, CNavigation* pNavigation, CModel* pModelCom)
    : m_pTransformCom { pTransform }
    , m_pNavigationCom { pNavigation }
    , m_pModelCom { pModelCom }
{
    Safe_AddRef(m_pTransformCom);
    Safe_AddRef(m_pNavigationCom);
    Safe_AddRef(m_pModelCom);
}

void CBird_AttackState::Start(_bool IsBlend)
{
    m_pModelCom->Set_AnimIndex("TenTailsCloneLoser02_Attack_Punch_Right", 1.f, true);
    m_pPlayerTransformCom = CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform();

    m_pTransformCom->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
    Safe_AddRef(m_pPlayerTransformCom);
}

CBirdState* CBird_AttackState::Update(_float fTimeDelta)
{
    CBirdState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pModelCom->Play_Animation(fTimeDelta);

    if (true == IsAnimFinished)
        pNextState = CBird_IdleState::Create(m_pTransformCom, m_pNavigationCom, m_pModelCom);

    return pNextState;
}

_bool CBird_AttackState::End()
{
    return true;
}

CBird_AttackState* CBird_AttackState::Create(CTransform* pTransform, CNavigation* pNavigation, CModel* pModelCom)
{
    return new CBird_AttackState(pTransform, pNavigation, pModelCom);
}

void CBird_AttackState::Free()
{
    __super::Free();

    Safe_Release(m_pTransformCom);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pPlayerTransformCom);
}
