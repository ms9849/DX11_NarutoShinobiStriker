#include "Player_RasenShurikenState.h"

#include "Player.h"
#include "RasenShuriken.h"
#include "GameInstance.h"
#include "GameManager.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"

#pragma endregion

/*
지상 나선 수리검.
크게 신경쓸 예외 사항은 없음
*/

CPlayer_RasenShurikenState::CPlayer_RasenShurikenState(CPlayer* pPlayer)
    : m_pPlayer { pPlayer }
{
    Safe_AddRef(m_pPlayer);
}

void CPlayer_RasenShurikenState::Start(_bool IsBlend)
{
    m_pPlayer->Set_Invincible(true);
    m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_TrueRasenShuriken", 1.5f);
}

CPlayerState* CPlayer_RasenShurikenState::Update(_float fTimeDelta)
{
    CPlayerState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPlayer->Get_AnimProgress();

    if (fAnimProgress >= 0.15f && false == m_IsShurikenCreate)
    {
        CRasenShuriken::RASENSHURIKEN_DESC Desc;
        Desc.fSpeedPerSec = 10.f;
        Desc.pSocketMatrix = m_pPlayer->Get_BoneMatrix(TEXT("Part_Upper"), "RightHandMiddle1");

        _vector vPosition = {};
        CTransform* pTargetTransform = CGameManager::GetInstance()->Calc_Target(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));

        /* 타겟이 없을때만 플레이어가 바라보는 방향으로 날아가게끔 한다. */
        if (nullptr != pTargetTransform)
            XMStoreFloat3(&Desc.vDir, vPosition - m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));
        /* 타겟이 있다면 타겟 방향으로 날아가게끔 한다. */
        else
            XMStoreFloat3(&Desc.vDir, m_pPlayer->Get_Transform()->Get_State(STATE::LOOK));

        m_pRasenShuriken = static_cast<CRasenShuriken*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC),
            TEXT("Prototype_GameObject_RasenShuriken"), &Desc));

        m_pGameInstance->Add_Clone_ToLayer(m_pRasenShuriken, m_pGameInstance->Get_LevelID(), TEXT("Layer_Skill"));
        Safe_AddRef(m_pRasenShuriken);

        m_IsShurikenCreate = true;
    }

    if (fAnimProgress >= 0.63f && false == m_IsShurikenThrow)
    {
        m_pRasenShuriken->Throw();
        m_IsShurikenThrow = true;
        m_pPlayer->Set_Invincible(false);
    }

    if (true == IsAnimFinished)
    {
        pNextState = CPlayer_IdleState::Create(m_pPlayer);
    }

    return pNextState;
}

_bool CPlayer_RasenShurikenState::End()
{
    m_pPlayer->Set_Invincible(false);
    return true;
}

CPlayer_RasenShurikenState* CPlayer_RasenShurikenState::Create(CPlayer* pPlayer)
{
    return new CPlayer_RasenShurikenState(pPlayer);
}

void CPlayer_RasenShurikenState::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
    Safe_Release(m_pRasenShuriken);
}
