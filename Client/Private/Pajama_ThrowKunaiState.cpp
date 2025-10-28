#include "Pajama_ThrowKunaiState.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Player.h"
#include "Pajama.h"
#include "Kunai.h"

#pragma region TRANSFER_STATE

#include "Pajama_IdleState.h"

#pragma endregion

CPajama_ThrowKunaiState::CPajama_ThrowKunaiState(CNavigation* pNavigation, CPajama* pPajama)
    : m_pNavigationCom{ pNavigation }
    , m_pPajama{ pPajama }
{
    Safe_AddRef(m_pNavigationCom);
}

void CPajama_ThrowKunaiState::Start(_bool IsBlend)
{
    m_pPajama->Set_AnimIndex("CustomMan_Throw_Kunai_Front", 2.f, IsBlend, 0.1f);
    m_pPajama->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
}

CPajamaState* CPajama_ThrowKunaiState::Update(_float fTimeDelta)
{
    CPajamaState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pPajama->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPajama->Get_AnimProgress();

    if (fAnimProgress >= 0.5f && false == m_IsKunaiThrow)
    {
        CKunai::KUNAI_DESC Desc;

        XMStoreFloat3(&Desc.vPosition, m_pPajama->Get_Transform()->Get_State(STATE::POSITION));
        XMStoreFloat3(&Desc.vDirection, m_pPajama->Get_Transform()->Get_State(STATE::LOOK));
        Desc.fSpeedPerSec = 30.f;
        Desc.eType = CKunai::KUNAI_TYPE::KUNAI;

        m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Kunai"),
            ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Layer_Skill"), &Desc);

        m_IsKunaiThrow = true;
    }

    if (true == IsAnimFinished)
    {
        pNextState = CPajama_IdleState::Create(m_pNavigationCom, m_pPajama);
    }

    return pNextState;
}

_bool CPajama_ThrowKunaiState::End()
{
    return true;
}

CPajama_ThrowKunaiState* CPajama_ThrowKunaiState::Create(CNavigation* pNavigation, CPajama* pPajama)
{
    return new CPajama_ThrowKunaiState(pNavigation, pPajama);
}

void CPajama_ThrowKunaiState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
