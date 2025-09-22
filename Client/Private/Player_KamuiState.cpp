#include "Player_KamuiState.h"

#include "Player.h"
#include "GameInstance.h"
#include "GameManager.h"

#include "Kamui.h"
/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"

#pragma endregion

CPlayer_KamuiState::CPlayer_KamuiState(CPlayer* pPlayer)
    : m_pPlayer { pPlayer }
    , m_pGameManager { CGameManager::GetInstance() }
{
    Safe_AddRef(m_pPlayer);
    Safe_AddRef(m_pGameManager);
}

void CPlayer_KamuiState::Start(_bool IsBlend)
{
    m_pGameManager->Change_Camera(LEVEL::GAMEPLAY, TEXT("Kamui_Action_Camera"));
    m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_KamuiKakashi", 1.5f, true);
    m_pPlayer->Set_Invincible(true);
}

CPlayerState* CPlayer_KamuiState::Update(_float fTimeDelta)
{
    CPlayerState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPlayer->Get_AnimProgress();

    if (fAnimProgress >= 0.8f && false == m_isKamuiThrow)
    {
        CKamui::KAMUI_DESC Desc;

        XMStoreFloat3(&Desc.vPosition, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));
        XMStoreFloat3(&Desc.vLook, m_pPlayer->Get_Transform()->Get_State(STATE::LOOK));

        m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Kamui"),
            m_pGameInstance->Get_LevelID(), TEXT("Layer_Skill"), &Desc);

        m_isKamuiThrow = true;
        m_pGameManager->Change_Camera(LEVEL::GAMEPLAY, TEXT("Main_Camera"));

    }

    if (true == IsAnimFinished)
        pNextState = CPlayer_IdleState::Create(m_pPlayer);

    return pNextState;
}

_bool CPlayer_KamuiState::End()
{
    m_pPlayer->Set_Invincible(false);

    return true;
}

CPlayer_KamuiState* CPlayer_KamuiState::Create(CPlayer* pPlayer)
{
    return new CPlayer_KamuiState(pPlayer);
}

void CPlayer_KamuiState::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
    Safe_Release(m_pGameManager);
}
