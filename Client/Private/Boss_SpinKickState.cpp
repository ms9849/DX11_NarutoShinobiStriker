#include "Boss_SpinKickState.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Boss.h"

#pragma region TRANSFER_STATE

#include "Boss_IdleState.h"
#include "Boss_StepState.h"

#pragma endregion	

#include "ParticleObject.h"
#include "Effect_HitSprite.h"
#include "Player.h"

CBoss_SpinKickState::CBoss_SpinKickState(CNavigation* pNavigation, CBoss* pBoss)
	: m_pBoss { pBoss}
	, m_pNavigationCom{ pNavigation }
{
	Safe_AddRef(m_pNavigationCom);
}

void CBoss_SpinKickState::Start(_bool IsBlend)
{
	m_pBoss->Set_AnimIndex("CustomMan_Ninjutsu_LeafHurricane_Start", 1.5f, IsBlend, 0.1f, false);
	m_eAnimState = ANIM_STATE::ATTACK_START;

}

CBossState* CBoss_SpinKickState::Update(_float fTimeDelta)
{
	CBossState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pBoss->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pBoss->Get_AnimProgress();
	_float fDist = XMVectorGetX(XMVector3Length(m_pBoss->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));

	Update_Collider(fAnimProgress);

	if (ANIM_STATE::ATTACK_START == m_eAnimState)
	{
		m_pBoss->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
		m_pBoss->Get_Transform()->Chase_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION), 
			fTimeDelta * 2.f, m_pNavigationCom);
	}

	/* 나뭇잎 선풍 시작 상태에다가, 달려가는 중에 플레이어랑 가깝다면 */
	if (ANIM_STATE::ATTACK_START == m_eAnimState
		&& (fDist <= 1.2f))
	{
		m_pBoss->Set_AnimIndex("CustomMan_Ninjutsu_D54NJ3_SpinkickEnd", 2.f, false, 0.1f, true);
		m_eAnimState = ANIM_STATE::ATTACK_END;
	}
	/* 나뭇잎 선풍 달려가는 중*/
	else if (ANIM_STATE::ATTACK_START == m_eAnimState
		&& true == IsAnimFinished)
	{
		m_pBoss->Set_AnimIndex("CustomMan_Ninjutsu_LeafHurricane_Start", 1.f, false, 0.1f, false);
		m_pBoss->Set_AnimProgress(1.0f);
		//조건문 추가
		if (false == m_IsSoundPlayed)
		{
			m_pGameInstance->PlaySoundOnce(TEXT("Boss_Osoi.wav"), CHANNELID::EFFECT2, 0.6f);
			m_IsSoundPlayed = true;
		}
	}
	else if (ANIM_STATE::ATTACK_END == m_eAnimState
		&& true == IsAnimFinished)
	{
		pNextState = CBoss_StepState::Create(m_pNavigationCom, m_pBoss, CBoss_StepState::ANIM_STATE::BACK);
	}

	// 콜라이더 On 기능 됐는데 꺼져있고, 파티클 만들어진적 없으면
	if (true == m_IsOnCollider && false == m_pBoss->Get_Collider(TEXT("Com_Collider_SpinKick"))->Get_Active() && false == m_IsParticleCreated)
	{
		m_pGameInstance->PlaySoundOnce(TEXT("Boss_Spinkick.wav"), CHANNELID::EFFECT2, 0.49f);
		/* 여기서 파티클 추가 */
		CParticleObject::PARTICLE_LOAD_DESC Desc;
		Desc.strParticlePath = TEXT("../Bin/Resources/Particle/BossSkill_Particle.bin");
		Desc.eType = CParticleObject::PARTICLE_TYPE::EXPLOSION;

		XMStoreFloat3(&Desc.vPosition, m_pGameManager->Get_PlayerPtr()->Get_Transform()->Get_State(STATE::POSITION));

		m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"), m_pGameInstance->Get_LevelID(),
			TEXT("Layer_Particle"), &Desc);

		CEffect_HitSprite::EFFECT_HIT_SPRITE_DESC HitDesc;

		HitDesc.fStartScale = 1.f;
		HitDesc.fDeltaScale = 40.f;
		HitDesc.fLifeTime = 0.f;
		HitDesc.IsBlur = true;
		HitDesc.iTextureNum = 0;
		HitDesc.vMainColor = _float4(1.0f, 0.6f, 0.2f, 0.5f);
		HitDesc.vSubColor = _float4(1.0f, 0.6f, 0.2f, 0.5f);
		XMStoreFloat4(&HitDesc.vPosition, m_pGameManager->Get_PlayerPtr()->Get_Transform()->Get_State(STATE::POSITION));

		m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_HitEffect"),
			m_pGameInstance->Get_LevelID(), TEXT("Layer_Effect"), &HitDesc);

		m_IsParticleCreated = true;
	}

	return pNextState;
}

_bool CBoss_SpinKickState::End()
{
	return true;
}

void CBoss_SpinKickState::Update_Collider(_float fAnimProgress)
{
	if (fAnimProgress >= 0.7f)
		m_pBoss->Get_Collider(TEXT("Com_Collider_SpinKick"))->Set_Active(false);

	else if (false == m_IsOnCollider && m_eAnimState == ANIM_STATE::ATTACK_END)
	{
		m_pBoss->Get_Collider(TEXT("Com_Collider_SpinKick"))->Set_Active(true);
		m_IsOnCollider = true;
	}
}

CBoss_SpinKickState* CBoss_SpinKickState::Create(CNavigation* pNavigation, CBoss* pBoss)
{
	return new CBoss_SpinKickState(pNavigation, pBoss);
}

void CBoss_SpinKickState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
