#include "WhiteJetsu_RunState.h"

#include "WhiteJetsu.h"
#include "Player.h"

#include "GameManager.h"
#include "GameInstance.h"

#pragma region TRANSFER_STATE

#include "WhiteJetsu_AttackState.h"
#include "WhiteJetsu_IdleState.h"

#pragma endregion

CWhiteJetsu_RunState::CWhiteJetsu_RunState(class CTransform* pTransform, class CNavigation* pNavigation, class CModel* pModelCom)
    : m_pTransformCom { pTransform }
    , m_pModelCom { pModelCom }
    , m_pNavigationCom { pNavigation }
{
    Safe_AddRef(m_pTransformCom);
    Safe_AddRef(m_pModelCom);
    Safe_AddRef(m_pNavigationCom);
}

void CWhiteJetsu_RunState::Start(_bool IsBlend)
{
    m_pModelCom->Set_AnimIndex("WhiteZetsuCrowdForm_Run_Loop");
    m_pPlayerTransformCom = CGameManager::GetInstance()->Get_PlayerPtr()->Get_PlayerTransformPtr();
    Safe_AddRef(m_pPlayerTransformCom);
}

CWhiteJetsuState* CWhiteJetsu_RunState::Update(_float fTimeDelta)
{
    CWhiteJetsuState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pModelCom->Play_Animation(fTimeDelta);

    m_pTransformCom->Chase(m_pPlayerTransformCom->Get_State(STATE::POSITION), fTimeDelta, m_pNavigationCom, 1.f);
    m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(STATE::POSITION));

    if (1.f >= XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION))))
    {
        pNextState = CWhiteJetsu_AttackState::Create(m_pTransformCom, m_pNavigationCom, m_pModelCom);
    }


    return pNextState;
}

_bool CWhiteJetsu_RunState::End()
{
    return true;
}

CWhiteJetsu_RunState* CWhiteJetsu_RunState::Create(class CTransform* pTransform, class CNavigation* pNavigation, class CModel* pModelCom)
{
    return new CWhiteJetsu_RunState(pTransform, pNavigation, pModelCom);
}

void CWhiteJetsu_RunState::Free()
{
    __super::Free();

    Safe_Release(m_pPlayerTransformCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pNavigationCom);
}
