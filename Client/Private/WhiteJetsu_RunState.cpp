#include "WhiteJetsu_RunState.h"

#include "WhiteJetsu.h"
#include "Player.h"

#include "GameManager.h"
#include "GameInstance.h"

#pragma region TRANSFER_STATE

#include "WhiteJetsu_AttackState.h"
#include "WhiteJetsu_IdleState.h"
#include "WhiteJetsu_WoodHandState.h"

#pragma endregion

CWhiteJetsu_RunState::CWhiteJetsu_RunState(class CNavigation* pNavigation, class CWhiteJetsu* pJetsu)
    : m_pJetsu { pJetsu }
    , m_pNavigationCom { pNavigation }
{
    Safe_AddRef(m_pNavigationCom);
}

void CWhiteJetsu_RunState::Start(_bool IsBlend)
{
    m_pJetsu->Set_AnimIndex("WhiteZetsuCrowdForm_Run_Loop");
    m_pPlayerTransformCom = CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform();
    Safe_AddRef(m_pPlayerTransformCom);
}

CWhiteJetsuState* CWhiteJetsu_RunState::Update(_float fTimeDelta)
{
    CWhiteJetsuState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pJetsu->Play_Animation(fTimeDelta);

    m_pJetsu->Get_Transform()->Chase_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION), fTimeDelta, m_pNavigationCom, 1.f);
    m_pJetsu->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));

    _float fDist = XMVectorGetX(XMVector3Length(m_pJetsu->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));


    if (fDist <= 1.f)
        pNextState = CWhiteJetsu_AttackState::Create(m_pNavigationCom, m_pJetsu);

    else if (fDist <= 20.f && true == m_pJetsu->Use_Skill())
        pNextState = CWhiteJetsu_WoodHandState::Create(m_pNavigationCom, m_pJetsu);

    return pNextState;
}

_bool CWhiteJetsu_RunState::End()
{
    return true;
}

CWhiteJetsu_RunState* CWhiteJetsu_RunState::Create(class CNavigation* pNavigation, class CWhiteJetsu* pJetsu)
{
    return new CWhiteJetsu_RunState(pNavigation, pJetsu);
}

void CWhiteJetsu_RunState::Free()
{
    __super::Free();

    Safe_Release(m_pPlayerTransformCom);
    Safe_Release(m_pNavigationCom);
}
