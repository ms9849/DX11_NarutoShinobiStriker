#include "Bird_BeatenBlastedState.h"

#include "Bird.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Bird_IdleState.h"

#pragma endregion

CBird_BeatenBlastedState::CBird_BeatenBlastedState(CNavigation* pNavigation, CBird* pBird, _vector vDirection, _float fRatio)
    : m_pNavigationCom { pNavigation}
    , m_pBird { pBird }

{
    Safe_AddRef(m_pNavigationCom);
    XMStoreFloat3(&m_vDirection, vDirection);
    m_vDirection.y = 0.f;
}

void CBird_BeatenBlastedState::Start(_bool IsBlend)
{
    /* Beaten보다 훨씬 멀리 날아가야 함 */
    m_pBird->Set_AnimIndex("TenTailsCloneLoser02_Beaten_Blasted", 2.f, true);

    m_pPlayerTransformCom = CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform();
    Safe_AddRef(m_pPlayerTransformCom);

    m_pBird->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
}

CBirdState* CBird_BeatenBlastedState::Update(_float fTimeDelta)
{
    CBirdState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pBird->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pBird->Get_AnimProgress();


    if (false == IsAnimFinished && fAnimProgress <= 0.7f)
        m_pBird->Get_Transform()->Go_Direction(XMLoadFloat3(&m_vDirection), 1.2f * fTimeDelta * m_pGameInstance->Calc_Quadratic(-2.f, 0.f, 1.f, fAnimProgress),
            m_pNavigationCom);

    if (true == IsAnimFinished)
    {
        pNextState = CBird_IdleState::Create(m_pNavigationCom, m_pBird);
    }

    return pNextState;
}

_bool CBird_BeatenBlastedState::End()
{
    return true;
}

CBird_BeatenBlastedState* CBird_BeatenBlastedState::Create(CNavigation* pNavigation, CBird* pBird, _vector vDirection, _float fRatio)
{
    return new CBird_BeatenBlastedState(pNavigation, pBird, vDirection);
}

void CBird_BeatenBlastedState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pPlayerTransformCom);
}
