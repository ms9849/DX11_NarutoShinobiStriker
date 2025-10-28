#include "Bird_AttackState.h"

#include "Bird.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Kunai.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Bird_IdleState.h"
#include "Bird_RunState.h"

#pragma endregion

CBird_AttackState::CBird_AttackState(CNavigation* pNavigation, CBird* pBird)
    : m_pNavigationCom { pNavigation }
    , m_pBird { pBird }
{
    Safe_AddRef(m_pNavigationCom);
}

void CBird_AttackState::Start(_bool IsBlend)
{
    m_pBird->Set_AnimIndex("TenTailsCloneLoser02_Attack_Punch_Right", 1.f, true);
    m_pPlayerTransformCom = CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform();

    m_pBird->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
    Safe_AddRef(m_pPlayerTransformCom);
}

CBirdState* CBird_AttackState::Update(_float fTimeDelta)
{
    CBirdState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pBird->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pBird->Get_AnimProgress();


    if (0.6 <= fAnimProgress && false == m_isThrow)
    {
        CKunai::KUNAI_DESC Desc;

        XMStoreFloat3(&Desc.vPosition, m_pBird->Get_Transform()->Get_State(STATE::POSITION));
        XMStoreFloat3(&Desc.vDirection, m_pBird->Get_Transform()->Get_State(STATE::LOOK));
        Desc.fSpeedPerSec = 30.f;
        Desc.eType = CKunai::KUNAI_TYPE::BIRD;

        m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Kunai"),
            ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Layer_Skill"), &Desc);
    
        m_isThrow = true;
    }

    if (true == IsAnimFinished)
        pNextState = CBird_IdleState::Create(m_pNavigationCom, m_pBird);

    return pNextState;
}

_bool CBird_AttackState::End()
{
    return true;
}

CBird_AttackState* CBird_AttackState::Create(CNavigation* pNavigation, CBird* pBird)
{
    return new CBird_AttackState(pNavigation, pBird);
}

void CBird_AttackState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pPlayerTransformCom);
}
