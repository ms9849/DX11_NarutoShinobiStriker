#include "Player_FireBallState.h"

#include "Player.h"
#include "GameInstance.h"

#include "GameManager.h"

#include "FireBall.h"
/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"
#include "Player_RunState.h"
#include "Player_StepState.h"
#include "Player_JumpState.h"

#pragma endregion

CPlayer_FireBallState::CPlayer_FireBallState(CPlayer* pPlayer)
    : m_pPlayer { pPlayer }
    , m_pGameManager { CGameManager::GetInstance() }
{
    Safe_AddRef(m_pPlayer);
    Safe_AddRef(m_pGameManager);
}

void CPlayer_FireBallState::Start(_bool IsBlend)
{
    m_pGameManager->Change_Camera(static_cast<LEVEL>(m_pGameInstance->Get_LevelID()), TEXT("FireBall_Action_Camera"), m_pGameInstance->Get_PipeLine_InverseFloat4x4(D3DTS::VIEW));
    m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Fireball_Lv3", 1.75f, true);

    m_pGameInstance->PlaySoundOnce(TEXT("Fireball_Voice.wav"), CHANNELID::EFFECT, 0.7f);
    m_pGameInstance->PlaySoundOnce(TEXT("Fireball_Loop.wav"), CHANNELID::EFFECT9, 0.5f);
}

CPlayerState* CPlayer_FireBallState::Update(_float fTimeDelta)
{
    CPlayerState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPlayer->Get_AnimProgress();

    if (false == m_IsFireballThrow && fAnimProgress >= 0.7f)
    {
        CFireBall::FIREBALL_DESC Desc;

        XMStoreFloat3(&Desc.vPosition, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));

        CTransform* pTargetTransform = m_pGameManager->Calc_Target(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));

        _vector vPosition = {};

        /* 타겟이 없을때만 플레이어가 바라보는 방향으로 날아가게끔 한다. */
        if (nullptr == pTargetTransform)
            vPosition = m_pPlayer->Get_Transform()->Get_State(STATE::LOOK);
        /* 타겟이 있다면 타겟 방향으로 날아가게끔 한다. */
        else
            vPosition =  pTargetTransform->Get_State(STATE::POSITION) - m_pPlayer->Get_Transform()->Get_State(STATE::POSITION);

        XMStoreFloat3(&Desc.vLook, vPosition);
        Desc.fSpeedPerSec = 15.f;

        m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_FireBall"),
            m_pGameInstance->Get_LevelID(), TEXT("Layer_Skill"), &Desc);

        m_IsFireballThrow = true;
    }

    if (true == IsAnimFinished)
        pNextState = CPlayer_IdleState::Create(m_pPlayer);

    else if ((m_pGameInstance->Key_Down(DIK_W) ||
        m_pGameInstance->Key_Down(DIK_A) ||
        m_pGameInstance->Key_Down(DIK_D))
        && fAnimProgress >= 0.8f)
        pNextState = CPlayer_RunState::Create(m_pPlayer);

    else if (m_pGameInstance->Key_Down(DIK_SPACE) && fAnimProgress >= 0.8f)
        pNextState = CPlayer_JumpState::Create(m_pPlayer);

    // 백스텝
    else if (m_pGameInstance->Key_Pressing(DIK_S) && fAnimProgress >= 0.8f)
    {
        // 돌다가 스텝 밟으면 상태 변경
        if (m_pGameInstance->Key_Down(DIK_LSHIFT))
            pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::BACK);
    }

    // 왼쪽 스텝
    else if (m_pGameInstance->Key_Pressing(DIK_A) && fAnimProgress >= 0.8f)
    {
        // 돌다가 스텝 밟으면 상태 변경
        if (m_pGameInstance->Key_Down(DIK_LSHIFT))
            pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::LEFT);
    }

    // 오른쪽 스텝
    else if (m_pGameInstance->Key_Pressing(DIK_D) && fAnimProgress >= 0.8f)
    {
        // 돌다가 스텝 밟으면 상태 변경
        if (m_pGameInstance->Key_Down(DIK_LSHIFT))
            pNextState = CPlayer_StepState::Create(m_pPlayer, CPlayer_StepState::ANIM_STATE::RIGHT);
    }

    return pNextState;
}

_bool CPlayer_FireBallState::End()
{
    return true;
}

CPlayer_FireBallState* CPlayer_FireBallState::Create(CPlayer* pPlayer)
{
    return new CPlayer_FireBallState(pPlayer);
}

void CPlayer_FireBallState::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
    Safe_Release(m_pGameManager);
}
