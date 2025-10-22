#include "Player_DuckingState.h"

#include "Player.h"
#include "GameInstance.h"
#include "GameManager.h"

#include "EffectContainer.h"
#include "EffectObject.h"
#include "ParticleObject.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_SuperJumpState.h"
#include "Player_IdleState.h"

#pragma endregion

CPlayer_DuckingState::CPlayer_DuckingState(CPlayer* pPlayer)
	: m_pPlayer { pPlayer }
{
	Safe_AddRef(m_pPlayer);
}

void CPlayer_DuckingState::Start(_bool IsBlend)
{
	m_pPlayer->Set_AnimIndex("CustomMan_ChakraJump_Charge_Start", 1.f, IsBlend);
	m_eAnimState = ANIM_STATE::START;

	/* 아기상어뚜루루뚜루 */
	CEffectContainer::EFFECT_CONTAINER_DESC EffectDesc;
	EffectDesc.IsBinary = true;
	EffectDesc.strFilePath = TEXT("../Bin/Resources/Effects/JumpSphere_eff.bin");
	m_pEffectMain = static_cast<CEffectContainer*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EffectContainer"),
		&EffectDesc));
	Safe_AddRef(m_pEffectMain);
	m_pGameInstance->Add_Clone_ToLayer(m_pEffectMain, m_pGameInstance->Get_LevelID(), TEXT("Layer_Effect"));

	EffectDesc.strFilePath = TEXT("../Bin/Resources/Effects/JumpLine_eff.bin");
	m_pEffectSub = static_cast<CEffectContainer*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EffectContainer"),
		&EffectDesc));
	Safe_AddRef(m_pEffectSub);
	m_pGameInstance->Add_Clone_ToLayer(m_pEffectSub, m_pGameInstance->Get_LevelID(), TEXT("Layer_Effect"));
}

CPlayerState* CPlayer_DuckingState::Update(_float fTimeDelta)
{
	CPlayerState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);

	m_pEffectMain->Set_ParentMatrix(XMMatrixTranslation(0, 1.f, 0.f) * XMLoadFloat4x4(m_pPlayer->Get_Transform()->Get_WorldMatrixPtr()));
	m_pEffectSub->Set_ParentMatrix(XMMatrixScaling(2.f, 8.f, 2.f) *  XMMatrixTranslation(0, 2.3f, 0.f) *  XMLoadFloat4x4(m_pPlayer->Get_Transform()->Get_WorldMatrixPtr()));


	if (m_pGameInstance->Key_Pressing(DIK_LCONTROL))
	{
		m_fTimeAcc += fTimeDelta;

		if (m_fTimeAcc > 2.f)
		{
			m_fTimeAcc = 2.f;

			if (false == m_IsChargeEnd)
			{
				m_IsChargeEnd = true; 

				CParticleObject::PARTICLE_LOAD_DESC Desc;
				Desc.strParticlePath = TEXT("../Bin/Resources/Particle/JumpCharge_Particle.bin");
				Desc.eType = CParticleObject::PARTICLE_TYPE::EXPLOSION_NON_UV;
				XMStoreFloat3(&Desc.vPosition, CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform()->Get_State(STATE::POSITION));

				m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"), m_pGameInstance->Get_LevelID(),
					TEXT("Layer_Particle"), &Desc);
			}
		}
	}
	else if(m_fTimeAcc < 2.f)
	{
		pNextState = CPlayer_IdleState::Create(m_pPlayer);
	}
	else if(m_fTimeAcc >= 2.f)
	{
		pNextState = CPlayer_SuperJumpState::Create(m_pPlayer, m_fTimeAcc);
	}
	/* 시작 애니메이션에서 루프로 */
	if (true == IsAnimFinished && ANIM_STATE::START == m_eAnimState)
	{
		m_pPlayer->Set_AnimIndex("CustomMan_ChakraJump_Charge_Loop", 1.f, false);
		m_eAnimState = ANIM_STATE::LOOP;
	}
	/* 루프 무한 반복 */
	else if (true == IsAnimFinished && ANIM_STATE::LOOP == m_eAnimState)
		m_pPlayer->Set_AnimIndex("CustomMan_ChakraJump_Charge_Loop", 1.f, false, 0.f, true);

	return pNextState;
}

_bool CPlayer_DuckingState::End()
{
	m_pEffectMain->Set_Dead(true);
	m_pEffectSub->Set_Dead(true);

	return true;
}

CPlayer_DuckingState* CPlayer_DuckingState::Create(CPlayer* pPlayer)
{
	return new CPlayer_DuckingState(pPlayer);
}

void CPlayer_DuckingState::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
	Safe_Release(m_pEffectMain);
	Safe_Release(m_pEffectSub); 
}
