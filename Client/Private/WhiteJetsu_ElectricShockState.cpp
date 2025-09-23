#include "WhiteJetsu_ElectricShockState.h"

#include "WhiteJetsu.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "WhiteJetsu_IdleState.h"

#pragma endregion

CWhiteJetsu_ElectricShockState::CWhiteJetsu_ElectricShockState(CNavigation* pNavigation, CWhiteJetsu* pJetsu)
    : m_pJetsu{ pJetsu }
    , m_pNavigationCom{ pNavigation }
    , m_pGameManager { CGameManager::GetInstance() }
{
    Safe_AddRef(m_pNavigationCom);
    Safe_AddRef(m_pGameManager);
}

void CWhiteJetsu_ElectricShockState::Start(_bool IsBlend)
{
    /* Beaten보다 훨씬 멀리 날아가야 함 */
    m_pJetsu->Set_AnimIndex("WhiteZetsuCrowdForm_Beaten_ElectricShock_Loop", 2.f, true);
}

CWhiteJetsuState* CWhiteJetsu_ElectricShockState::Update(_float fTimeDelta)
{
    CWhiteJetsuState* pNextState = { nullptr };
    _bool  IsAnimFinished = m_pJetsu->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pJetsu->Get_AnimProgress();

    m_fTimeAcc += fTimeDelta;

    if (m_fMaxTimeAcc <= m_fTimeAcc)
    {
        pNextState = CWhiteJetsu_IdleState::Create(m_pNavigationCom, m_pJetsu);
    }

    return pNextState;
}

_bool CWhiteJetsu_ElectricShockState::End()
{
    return true;
}

CWhiteJetsu_ElectricShockState* CWhiteJetsu_ElectricShockState::Create(CNavigation* pNavigation, CWhiteJetsu* pJetsu)
{
    return new CWhiteJetsu_ElectricShockState(pNavigation, pJetsu);
}

void CWhiteJetsu_ElectricShockState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pGameManager);
}
