#include "Player_RasenganState.h"

#include "Player.h"
#include "Rasengan.h"
#include "GameInstance.h"
#include "GameManager.h"

#include "ParticleObject.h"
/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

// 나선환 여기서 끝내
#include "Player_IdleState.h"

#pragma endregion

#include "Effect_HitSprite.h"

CPlayer_RasenganState::CPlayer_RasenganState(CPlayer* pPlayer, CRasengan* pRasengan)
	: m_pPlayer { pPlayer }
    , m_pGameManager { CGameManager::GetInstance() }
    , m_pRasengan { pRasengan }
{
	Safe_AddRef(m_pPlayer);
    Safe_AddRef(m_pGameManager);
    Safe_AddRef(m_pRasengan);
}

void CPlayer_RasenganState::Start(_bool IsBlend)
{
	m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_Rasengun_Charge_Lv2toLv3_Conect_toRun", 1.5f, true);
	m_eAnimState = ANIM_STATE::ATTACK_START;
    m_pRasengan->Toggle_Effect();

    m_pGameInstance->PlaySoundOnce(TEXT("Rasengan_Voice.wav"), CHANNELID::EFFECT, 0.7f);
}

CPlayerState* CPlayer_RasenganState::Update(_float fTimeDelta)
{
	CPlayerState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPlayer->Get_AnimProgress();

    m_fTimeAcc += fTimeDelta;

    // 추적
    CTransform* pTargetTransform = m_pGameManager->Calc_Target(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));

    if (nullptr != pTargetTransform && ANIM_STATE::ATTACK_END != m_eAnimState)
    {
        _vector vTargetPosition = pTargetTransform->Get_State(STATE::POSITION);
        m_pPlayer->Get_Transform()->LookAt_Lerp(vTargetPosition);
    }

    // 머리 회전
    else if ((m_pGameInstance->Key_Pressing(DIK_A) ||
        m_pGameInstance->Key_Pressing(DIK_D))
        && ANIM_STATE::ATTACK_END != m_eAnimState)
    {
        if (m_pGameInstance->Key_Pressing(DIK_D))
            m_pPlayer->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 0.5f);

        if (m_pGameInstance->Key_Pressing(DIK_A))
            m_pPlayer->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -0.5f);
    }

    // 나선환 차징 -> 달리기 끝났다면
    if (true == IsAnimFinished && ANIM_STATE::ATTACK_START == m_eAnimState)
    {
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

        CParticleObject::PARTICLE_LOAD_DESC Desc;
        Desc.strParticlePath = TEXT("../Bin/Resources/Particle/RasenganHit_Particle.bin");
        Desc.eType = CParticleObject::PARTICLE_TYPE::EXPLOSION_NON_UV;
        XMStoreFloat3(&Desc.vPosition, m_pGameManager->Get_PlayerPtr()->Get_Transform()->Get_State(STATE::POSITION));

        m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"), m_pGameInstance->Get_LevelID(),
            TEXT("Layer_Particle"), &Desc);

        Desc.strParticlePath = TEXT("../Bin/Resources/Particle/Rasengan_Explosion_Smoke_Particle.bin");
        Desc.eType = CParticleObject::PARTICLE_TYPE::EXPLOSION;
        Desc.IsBlur = false;

        m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"), m_pGameInstance->Get_LevelID(),
            TEXT("Layer_Particle"), &Desc);
    }


    // IDLE 상태로 전환
    else if (true == IsAnimFinished && ANIM_STATE::ATTACK_END == m_eAnimState)
    {
        pNextState = CPlayer_IdleState::Create(m_pPlayer);
    }

    // 나선환 사용중 바라보는 방향으로 날아가기
    if (ANIM_STATE::ATTACK == m_eAnimState)
        m_pPlayer->Get_Transform()->Go_Direction(m_pPlayer->Get_Transform()->Get_State(STATE::LOOK), fTimeDelta * 1.5f,
            nullptr);

    else if (ANIM_STATE::ATTACK_START == m_eAnimState)
        m_pPlayer->Get_Transform()->Go_Direction(m_pPlayer->Get_Transform()->Get_State(STATE::LOOK), fTimeDelta * m_pGameInstance->Calc_Linear(1.5f, 0.f, fAnimProgress),
            nullptr);

    else if (false == IsAnimFinished && fAnimProgress < 0.7f && ANIM_STATE::ATTACK_END == m_eAnimState)
        m_pPlayer->Get_Transform()->Go_Direction(m_pPlayer->Get_Transform()->Get_State(STATE::LOOK), fTimeDelta * m_pGameInstance->Calc_Linear(-1.5f, 1.1f, fAnimProgress),
            nullptr);

	return pNextState;
}

_bool CPlayer_RasenganState::End()
{
    m_pPlayer->Set_Invincible(false);
    m_pRasengan->Set_Dead(true);

	return true;
}

CPlayer_RasenganState* CPlayer_RasenganState::Create(CPlayer* pPlayer, CRasengan* pRasengan)
{
	return new CPlayer_RasenganState(pPlayer, pRasengan);
}

void CPlayer_RasenganState::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
    Safe_Release(m_pRasengan);
    Safe_Release(m_pGameManager);
}
