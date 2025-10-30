#include "Pajama_DeadState.h"

#include "GameManager.h"
#include "GameInstance.h"
#include "Pajama.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#pragma endregion


CPajama_DeadState::CPajama_DeadState(CNavigation* pNavigation, CPajama* pPajama)
    : m_pPajama { pPajama}
    , m_pNavigationCom { pNavigation }
{
    Safe_AddRef(m_pNavigationCom);
}

void CPajama_DeadState::Start(_bool IsBlend)
{
    m_pGameManager->Active_KO();
    m_pPajama->Set_AnimIndex("CustomMan_Dying_Type01", 1.5f, true);

    m_pGameInstance->PlaySoundOnce(TEXT("Pajama_Die.wav"), CHANNELID::EFFECT, 0.45f);
}

CPajamaState* CPajama_DeadState::Update(_float fTimeDelta)
{
    CPajamaState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pPajama->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPajama->Get_AnimProgress();

    if (true == IsAnimFinished)
    {
        m_pPajama->Fade_Particle();
        m_pPajama->Set_Dead(true);
    }

    return nullptr;
}

_bool CPajama_DeadState::End()
{
    return false;
}

CPajama_DeadState* CPajama_DeadState::Create(CNavigation* pNavigation, CPajama* pPajama)
{
    return new CPajama_DeadState(pNavigation, pPajama);
}

void CPajama_DeadState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
