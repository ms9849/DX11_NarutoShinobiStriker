#include "Player_RasenganState.h"

#include "Player.h"
#include "Rasengan.h"
#include "GameInstance.h"
#include "GameManager.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

// 나선환 여기서 끝내
#include "Player_IdleState.h"

#pragma endregion

CPlayer_RasenganState::CPlayer_RasenganState(CPlayer* pPlayer)
	: m_pPlayer { pPlayer }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_RasenganState::Start(_bool IsBlend)
{
	m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Rasengun_Charge_Lv2toLv3_Conect_toRun", 1.5f, true);
	m_eAnimState = ANIM_STATE::ATTACK_START;
}

CPlayerState* CPlayer_RasenganState::Update(_float fTimeDelta)
{
	CPlayerState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPlayer->Get_AnimProgress();

    m_fTimeAcc += fTimeDelta;

    // 대가리 회전
    if ((m_pGameInstance->Key_Pressing(DIK_A) ||
        m_pGameInstance->Key_Pressing(DIK_D))
        && ANIM_STATE::ATTACK_END != m_eAnimState)
    {
        if (m_pGameInstance->Key_Pressing(DIK_D))
            m_pPlayer->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 0.25f);

        if (m_pGameInstance->Key_Pressing(DIK_A))
            m_pPlayer->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -0.25f);
    }

    // 나선환 차징 -> 달리기 끝났다면
    if (true == IsAnimFinished && ANIM_STATE::ATTACK_START == m_eAnimState)
    {
        CRasengan::RASENGAN_DESC Desc;
        Desc.pSocketMatrix = m_pPlayer->Get_BoneMatrix(TEXT("Part_Upper"), "RightHandMiddle1");

        m_pRasengan = static_cast<CRasengan*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC),
            TEXT("Prototype_GameObject_Rasengan"), &Desc)); 

        m_pGameInstance->Add_Clone_ToLayer(m_pRasengan, m_pGameInstance->Get_LevelID(), TEXT("Layer_Skill"));
        Safe_AddRef(m_pRasengan);

        m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Rasengun_Run_Lv1_Loop", 1.5f, false, 0.f);
        m_eAnimState = ANIM_STATE::ATTACK;
        m_fTimeAcc = 0.f;
    }

    // 나선환 달리기 루프 시켜주기. 
    else if (true == IsAnimFinished && m_fTimeAcc < 1.2f && ANIM_STATE::ATTACK == m_eAnimState)
    {
        m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Rasengun_Run_Lv1_Loop", 1.5f, false, 0.f);
        m_pPlayer->Set_AnimProgress(0.084f);
    }

    // 나선환 RMx 애니메이션 전환. 나중에 맞았는지 체크해야함. 
    else if  (ANIM_STATE::ATTACK == m_eAnimState && (false == m_pRasengan->IsColliderActive() || m_fTimeAcc >= 1.2f))
    {
        m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Rasengun_Attack_Lv1_End", 1.f, false);
        m_eAnimState = ANIM_STATE::ATTACK_END;
    }


    // IDLE 상태로 전환
    else if (true == IsAnimFinished && ANIM_STATE::ATTACK_END == m_eAnimState)
    {
        pNextState = CPlayer_IdleState::Create(m_pPlayer);
    }

    // 나선환 사용중 바라보는 방향으로 날아가기
    if (ANIM_STATE::ATTACK == m_eAnimState)
        m_pPlayer->Get_Transform()->Go_Direction(m_pPlayer->Get_Transform()->Get_State(STATE::LOOK), fTimeDelta * 1.5f,
            m_pPlayer->Get_Navigation());

    else if (ANIM_STATE::ATTACK_START == m_eAnimState)
        m_pPlayer->Get_Transform()->Go_Direction(m_pPlayer->Get_Transform()->Get_State(STATE::LOOK), fTimeDelta * m_pGameInstance->Calc_Linear(1.5f, 0.f, fAnimProgress),
            m_pPlayer->Get_Navigation());

    else if (false == IsAnimFinished && fAnimProgress < 0.7f && ANIM_STATE::ATTACK_END == m_eAnimState)
        m_pPlayer->Get_Transform()->Go_Direction(m_pPlayer->Get_Transform()->Get_State(STATE::LOOK), fTimeDelta * m_pGameInstance->Calc_Linear(-1.5f, 1.1f, fAnimProgress),
            m_pPlayer->Get_Navigation());

	return pNextState;
}

_bool CPlayer_RasenganState::End()
{
	return true;
}

CPlayer_RasenganState* CPlayer_RasenganState::Create(CPlayer* pPlayer)
{
	return new CPlayer_RasenganState(pPlayer);
}

void CPlayer_RasenganState::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
    Safe_Release(m_pRasengan);
}
