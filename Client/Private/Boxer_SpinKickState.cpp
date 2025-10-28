#include "Boxer_SpinKickState.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Boxer.h"

#pragma region TRANSFER_STATE

#include "Boxer_AttackState.h"
#include "Boxer_IdleState.h"

#pragma endregion	

#include "ParticleObject.h"
#include "Effect_HitSprite.h"
#include "Player.h"

CBoxer_SpinKickState::CBoxer_SpinKickState(CNavigation* pNavigation, CBoxer* pBoxer)
	: m_pBoxer { pBoxer}
	, m_pNavigationCom { pNavigation}
{
	Safe_AddRef(m_pNavigationCom);
}

void CBoxer_SpinKickState::Start(_bool IsBlend)
{
	m_pBoxer->Set_AnimIndex("CustomMan_Ninjutsu_LeafHurricane_Start", 1.5f, IsBlend, 0.1f, false);
	m_eAnimState = ANIM_STATE::ATTACK_START;
}

CBoxerState* CBoxer_SpinKickState::Update(_float fTimeDelta)
{
	CBoxerState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pBoxer->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pBoxer->Get_AnimProgress();
	_float fDist = XMVectorGetX(XMVector3Length(m_pBoxer->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));

	Update_Collider(fAnimProgress);

	if (ANIM_STATE::ATTACK_START == m_eAnimState)
	{
		m_pBoxer->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
		m_pBoxer->Get_Transform()->Chase_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION), fTimeDelta * 2.f, m_pNavigationCom);
	}

	/* 나뭇잎 선풍 시작 상태에다가, 달려가는 중에 플레이어랑 가깝다면 */
	if (ANIM_STATE::ATTACK_START == m_eAnimState
		&& (fDist <= 1.2f))
	{
		m_pBoxer->Set_AnimIndex("CustomMan_Ninjutsu_D54NJ3_SpinkickEnd", 2.f, false, 0.1f, true);
		m_eAnimState = ANIM_STATE::ATTACK_END;
	}
	/* 나뭇잎 선풍 달려가는 중*/
	else if (ANIM_STATE::ATTACK_START == m_eAnimState
		&& true == IsAnimFinished)
	{
		m_pBoxer->Set_AnimIndex("CustomMan_Ninjutsu_LeafHurricane_Start", 1.f, false, 0.1f, false);
		m_pBoxer->Set_AnimProgress(1.0f);
	}
	else if (ANIM_STATE::ATTACK_END == m_eAnimState
		&& true == IsAnimFinished)
	{
		_float fRandom = m_pGameInstance->Random_Normal();
		pNextState = CBoxer_IdleState::Create(m_pNavigationCom, m_pBoxer);
	}

	// 콜라이더 On 기능 됐는데 꺼져있고, 파티클 만들어진적 없으면
	if (true == m_IsOnCollider && false == m_pBoxer->Get_Collider(TEXT("Com_Collider_SpinKick"))->Get_Active() && false == m_IsParticleCreated)
	{
		/* 여기서 파티클 추가 */
		CParticleObject::PARTICLE_LOAD_DESC Desc;
		Desc.strParticlePath = TEXT("../Bin/Resources/Particle/BoxerSkill_Particle.bin");
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
		HitDesc.vMainColor = _float4(0.5f, 1.f, 0.5f, 0.5f);
		HitDesc.vSubColor = _float4(0.5f, 1.f, 0.5f, 0.5f);
		XMStoreFloat4(&HitDesc.vPosition, m_pGameManager->Get_PlayerPtr()->Get_Transform()->Get_State(STATE::POSITION));

		m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_HitEffect"),
			m_pGameInstance->Get_LevelID(), TEXT("Layer_Effect"), &HitDesc);

		m_IsParticleCreated = true;
	}


	return pNextState;
}

_bool CBoxer_SpinKickState::End()
{
	m_pBoxer->Get_Collider(TEXT("Com_Collider_SpinKick"))->Set_Active(false);
	return true;
}

void CBoxer_SpinKickState::Update_Collider(_float fAnimProgress)
{
	if (fAnimProgress >= 0.7f)
		m_pBoxer->Get_Collider(TEXT("Com_Collider_SpinKick"))->Set_Active(false);

	else if (false == m_IsOnCollider && m_eAnimState == ANIM_STATE::ATTACK_END)
	{
		m_pBoxer->Get_Collider(TEXT("Com_Collider_SpinKick"))->Set_Active(true);
		m_IsOnCollider = true;
	}
}

CBoxer_SpinKickState* CBoxer_SpinKickState::Create(CNavigation* pNavigation, CBoxer* pBoxer)
{
	return new CBoxer_SpinKickState(pNavigation, pBoxer);
}

void CBoxer_SpinKickState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
