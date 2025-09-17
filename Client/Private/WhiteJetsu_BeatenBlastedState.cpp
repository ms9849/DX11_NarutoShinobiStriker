#include "WhiteJetsu_BeatenBlastedState.h"

#include "WhiteJetsu.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "WhiteJetsu_IdleState.h"

#pragma endregion

CWhiteJetsu_BeatenBlastedState::CWhiteJetsu_BeatenBlastedState(CNavigation* pNavigation, CWhiteJetsu* pJetsu, _vector vDirection)
    : m_pJetsu{ pJetsu }
    , m_pNavigationCom{ pNavigation }
{
    Safe_AddRef(m_pNavigationCom);
    XMStoreFloat3(&m_vDirection, vDirection);
    m_vDirection.y = 0.f;
}

void CWhiteJetsu_BeatenBlastedState::Start(_bool IsBlend)
{
    /* Beaten보다 훨씬 멀리 날아가야 함 */
    m_pJetsu->Set_AnimIndex("WhiteZetsuCrowdForm_Beaten_Blasted", 2.f, true);

    m_pPlayerTransformCom = CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform();
    Safe_AddRef(m_pPlayerTransformCom);

    m_pJetsu->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
}

CWhiteJetsuState* CWhiteJetsu_BeatenBlastedState::Update(_float fTimeDelta)
{
    CWhiteJetsuState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pJetsu->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pJetsu->Get_AnimProgress();

   
    if (false == IsAnimFinished && fAnimProgress <= 0.7f)
        m_pJetsu->Get_Transform()->Go_Direction(XMLoadFloat3(&m_vDirection), 1.2f * fTimeDelta * m_pGameInstance->Calc_Quadratic(-2.f, 0.f, 1.f, fAnimProgress),
            m_pNavigationCom);

    if (true == IsAnimFinished)
    {
        pNextState = CWhiteJetsu_IdleState::Create(m_pNavigationCom, m_pJetsu);
    }

    return pNextState;
}

_bool CWhiteJetsu_BeatenBlastedState::End()
{
    return true;
}

CWhiteJetsu_BeatenBlastedState* CWhiteJetsu_BeatenBlastedState::Create(CNavigation* pNavigation, CWhiteJetsu* pJetsu, _vector vDirection)
{
    return new CWhiteJetsu_BeatenBlastedState(pNavigation, pJetsu, vDirection);
}

void CWhiteJetsu_BeatenBlastedState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pPlayerTransformCom);
}