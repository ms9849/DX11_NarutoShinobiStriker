#include "WhiteJetsu_WoodHandState.h"

#include "WhiteJetsu.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "WoodHand.h"

/* 전이 가능한 상태들 */

#pragma region TRANSFER_STATE

#include "WhiteJetsu_IdleState.h"
#include "WhiteJetsu_RunState.h"

#pragma endregion

#include "ParticleObject.h"

CWhiteJetsu_WoodHandState::CWhiteJetsu_WoodHandState(CNavigation* pNavigation, CWhiteJetsu* pJetsu)
	: m_pJetsu{ pJetsu }
	, m_pNavigationCom{ pNavigation }
{
	Safe_AddRef(m_pNavigationCom);
}

void CWhiteJetsu_WoodHandState::Start(_bool IsBlend)
{
	m_pJetsu->Set_AnimIndex("WhiteZetsuCrowdForm_etc_Action_Type04", 1.f, true);

	m_pPlayerTransformCom = CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform();

	m_pJetsu->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
	Safe_AddRef(m_pPlayerTransformCom);
}

CWhiteJetsuState* CWhiteJetsu_WoodHandState::Update(_float fTimeDelta)
{
	CWhiteJetsuState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pJetsu->Play_Animation(fTimeDelta);

	if (true == IsAnimFinished)
	{
		CWoodHand::WOODHAND_DESC Desc{};
		Desc.fSpeedPerSec = 10.f;

		 XMStoreFloat3(&Desc.vTargetPos, m_pPlayerTransformCom->Get_State(STATE::POSITION));
		 Desc.vTargetPos.y = XMVectorGetY(m_pJetsu->Get_Transform()->Get_State(STATE::POSITION) - XMVectorSet(0.f, 1.f, 0.f, 0.f));

		 XMStoreFloat3(&Desc.vJetsuPos, m_pJetsu->Get_Transform()->Get_State(STATE::POSITION));

		m_pGameInstance->Add_GameObject_ToLayer(m_pGameInstance->Get_LevelID(), TEXT("Prototype_GameObject_WoodHand"),
			m_pGameInstance->Get_LevelID(), TEXT("Layer_Skills"), &Desc);

		CParticleObject::PARTICLE_LOAD_DESC ParticleDesc;
		ParticleDesc.strParticlePath = TEXT("../Bin/Resources/Particle/WoodHand_Particle.bin");
		ParticleDesc.eType = CParticleObject::PARTICLE_TYPE::EXPLOSION;
		XMStoreFloat3(&ParticleDesc.vPosition, XMVectorSet(0.f, 0.35f, 0.f, 0.f) + CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform()->Get_State(STATE::POSITION));
		ParticleDesc.IsBlur = false;

		m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"), m_pGameInstance->Get_LevelID(),
			TEXT("Layer_Particle"), &ParticleDesc);

		_float fDist = XMVectorGetX(XMVector3Length(m_pJetsu->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));

		if (fDist < 30.f)
			pNextState = CWhiteJetsu_RunState::Create(m_pNavigationCom, m_pJetsu);

		else
			pNextState = CWhiteJetsu_IdleState::Create(m_pNavigationCom, m_pJetsu);
	}

    return pNextState;
}

_bool CWhiteJetsu_WoodHandState::End()
{
    return true;
}

CWhiteJetsu_WoodHandState* CWhiteJetsu_WoodHandState::Create(CNavigation* pNavigation, CWhiteJetsu* pJetsu)
{
    return new CWhiteJetsu_WoodHandState(pNavigation, pJetsu);
}

void CWhiteJetsu_WoodHandState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pPlayerTransformCom);
}
