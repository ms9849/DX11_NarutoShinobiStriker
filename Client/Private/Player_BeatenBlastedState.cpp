#include "Player_BeatenBlastedState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"

#pragma endregion

CPlayer_BeatenBlastedState::CPlayer_BeatenBlastedState(CPlayer* pPlayer, _vector vDir, _float fRatio)
    : m_pPlayer { pPlayer }
    , m_fRatio { fRatio }
{
    XMStoreFloat3(&m_vDirection, vDir);
    Safe_AddRef(m_pPlayer);
}

void CPlayer_BeatenBlastedState::Start(_bool IsBlend)
{
    m_pPlayer->Set_AnimIndex("CustomMan_Beaten_Blasted", 1.5f, IsBlend);
}

CPlayerState* CPlayer_BeatenBlastedState::Update(_float fTimeDelta)
{
    CPlayerState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPlayer->Get_AnimProgress();

    if (false == IsAnimFinished && fAnimProgress <= 0.7f)
        m_pPlayer->Get_Transform()->Go_Direction(XMLoadFloat3(&m_vDirection), 1.6f * fTimeDelta * m_pGameInstance->Calc_Quadratic(-2.f, 0.f, 1.f, fAnimProgress),
            m_pPlayer->Get_Navigation());

    if (true == IsAnimFinished)
    {
        pNextState = CPlayer_IdleState::Create(m_pPlayer);
    }

    return pNextState;
}

_bool CPlayer_BeatenBlastedState::End()
{
    return true;
}

CPlayer_BeatenBlastedState* CPlayer_BeatenBlastedState::Create(CPlayer* pPlayer, _vector vDir, _float fRatio)
{
    return new CPlayer_BeatenBlastedState(pPlayer, vDir, fRatio);
}

void CPlayer_BeatenBlastedState::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
}
