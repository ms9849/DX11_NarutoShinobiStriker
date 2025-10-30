#include "Player_RasenganReadyState.h"

#include "Player.h"
#include "GameInstance.h"

#include "Rasengan.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

// 나선환 실제 공격으로 넘어가기
#include "Player_RasenganState.h"

#pragma endregion


CPlayer_RasenganReadyState::CPlayer_RasenganReadyState(CPlayer* pPlayer)
    : m_pPlayer { pPlayer }
{
    Safe_AddRef(m_pPlayer);
}

void CPlayer_RasenganReadyState::Start(_bool IsBlend)
{
    m_pPlayer->Set_Invincible(true);
    m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Rasengun_Charge_Lv2toLv3", 1.f, true);

    CRasengan::RASENGAN_DESC Desc;
    Desc.pSocketMatrix = m_pPlayer->Get_BoneMatrix(TEXT("Part_Upper"), "RightHandMiddle1");

    m_pRasengan = static_cast<CRasengan*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC),
        TEXT("Prototype_GameObject_Rasengan"), &Desc));

    m_pGameInstance->Add_Clone_ToLayer(m_pRasengan, m_pGameInstance->Get_LevelID(), TEXT("Layer_Skill"));
    Safe_AddRef(m_pRasengan);

    m_pGameInstance->PlaySoundOnce(TEXT("Rasengan_Create.wav"), CHANNELID::EFFECT_SPECIAL2, 0.2f);
}

CPlayerState* CPlayer_RasenganReadyState::Update(_float fTimeDelta)
{
    CPlayerState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);

    if (true == IsAnimFinished)
    {
        pNextState = CPlayer_RasenganState::Create(m_pPlayer, m_pRasengan);
    }

    return pNextState;
}

_bool CPlayer_RasenganReadyState::End()
{
    return true;
}

CPlayer_RasenganReadyState* CPlayer_RasenganReadyState::Create(CPlayer* pPlayer)
{
    return new CPlayer_RasenganReadyState(pPlayer);
}

void CPlayer_RasenganReadyState::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
    Safe_Release(m_pRasengan);
}
