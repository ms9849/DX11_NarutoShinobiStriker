#include "Player_BeatenBlastedState.h"

#include "Player.h"
#include "GameInstance.h"

/* ÀüÀÌ °¡´ÉÇÑ »óÅÂµé */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"
#include "Player_StepState.h"

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
            nullptr);

    if (true == IsAnimFinished)
    {
        pNextState = CPlayer_IdleState::Create(m_pPlayer);
    }
    // ¹é½ºÅÜ
    else if (m_pGameInstance->Key_Pressing(DIK_S) && m_pGameInstance->Key_Down(DIK_LSHIFT)
        && fAnimProgress >= 0.7f)
    {
        pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::BACK);
        m_IsNextAnimBlened = true;
    }
    // ¿À¸¥ÂÊ ½ºÅÜ
    else if (m_pGameInstance->Key_Pressing(DIK_D)
        && fAnimProgress >= 0.7f)
    {
        if (m_pGameInstance->Key_Down(DIK_LSHIFT) && nullptr == pNextState)
            pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::RIGHT);
    }
    // ¿ÞÂÊ ½ºÅÜ
    else if (m_pGameInstance->Key_Pressing(DIK_A)
        && fAnimProgress >= 0.7f)
    {
        if (m_pGameInstance->Key_Down(DIK_LSHIFT) && nullptr == pNextState)
            pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::LEFT);
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
