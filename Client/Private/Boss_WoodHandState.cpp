#include "Boss_WoodHandState.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Player.h"
#include "Boss.h"
#include "WoodHand.h"

#pragma region TRANSFER_STATE

#include "Boss_IdleState.h"
#include "Boss_StepState.h"
#include "Boss_LandState.h"

#pragma endregion

#include "ParticleObject.h"

CBoss_WoodHandState::CBoss_WoodHandState(CNavigation* pNavigation, CBoss* pBoss)
	: m_pBoss{ pBoss }
	, m_pNavigationCom{ pNavigation }
{
	Safe_AddRef(m_pNavigationCom);
}

void CBoss_WoodHandState::Start(_bool IsBlend)
{
	m_pBoss->Set_AnimIndex("CustomMan_Ninjutsu_LaughingMonk", 1.5f, true);
	m_pBoss->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));

	m_pGameInstance->PlaySoundOnce(TEXT("Boss_WoodHand.wav"), CHANNELID::EFFECT2, 0.49f);
}

CBossState* CBoss_WoodHandState::Update(_float fTimeDelta)
{
	CBossState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pBoss->Play_Animation(fTimeDelta);

	if (true == IsAnimFinished)
	{
		CWoodHand::WOODHAND_DESC Desc{};
		Desc.fSpeedPerSec = 10.f;

		XMStoreFloat3(&Desc.vTargetPos, m_pPlayerTransformCom->Get_State(STATE::POSITION));
		Desc.vTargetPos.y = XMVectorGetY(m_pBoss->Get_Transform()->Get_State(STATE::POSITION) - XMVectorSet(0.f, 1.f, 0.f, 0.f));

		XMStoreFloat3(&Desc.vJetsuPos, m_pBoss->Get_Transform()->Get_State(STATE::POSITION));

		m_pGameInstance->Add_GameObject_ToLayer(m_pGameInstance->Get_LevelID(), TEXT("Prototype_GameObject_WoodHand"),
			m_pGameInstance->Get_LevelID(), TEXT("Layer_Skills"), &Desc);

		CParticleObject::PARTICLE_LOAD_DESC ParticleDesc;
		ParticleDesc.strParticlePath = TEXT("../Bin/Resources/Particle/WoodHand_Particle.bin");
		ParticleDesc.eType = CParticleObject::PARTICLE_TYPE::EXPLOSION;
		XMStoreFloat3(&ParticleDesc.vPosition, XMVectorSet(0.f, 0.35f, 0.f, 0.f) + CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform()->Get_State(STATE::POSITION));
		ParticleDesc.IsBlur = false;

		m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"), m_pGameInstance->Get_LevelID(),
			TEXT("Layer_Particle"), &ParticleDesc);

		pNextState = CBoss_IdleState::Create(m_pNavigationCom, m_pBoss);
	}

	return pNextState;
}

_bool CBoss_WoodHandState::End()
{
	return true;
}

CBoss_WoodHandState* CBoss_WoodHandState::Create(CNavigation* pNavigation, CBoss* pBoss)
{
	return new CBoss_WoodHandState(pNavigation, pBoss);
}

void CBoss_WoodHandState::Free()
{
	__super::Free();
	Safe_Release(m_pNavigationCom);
}
