#include "Boxer.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Head_Character.h"
#include "Face_Character.h"
#include "Upper_Character.h"
#include "Lower_Character.h"
#include "Weapon_Character.h"

#include "Player.h"

#include "Boxer_IdleState.h"
#include "Boxer_BeatenBlastedState.h"
#include "Boxer_BeatenState.h"
#include "Boxer_DeadState.h"
#include "Boxer_ElectricShockState.h"

#include "Trail.h"

CBoxer::CBoxer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CEnemy { pDevice, pContext, eObjectID }
{
}

CBoxer::CBoxer(const CBoxer& rhs)
	: CEnemy{ rhs }
{
}

_float CBoxer::Get_AnimProgress()
{
	return dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Upper")))->Get_AnimProgress();
}

void CBoxer::Set_AnimProgress(_float fProgress)
{
	for (auto& pPart : m_PartObjects)
	{
		dynamic_cast<CParts_Character*>(pPart.second)->Set_AnimProgress(fProgress);
	}
}

void CBoxer::Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate, _bool IsBlend, _float fBlendRatio, _bool IsLoop)
{
	for (auto& pPart : m_PartObjects)
	{
		dynamic_cast<CParts_Character*>(pPart.second)->Set_AnimIndex(pAnimName, fAnimationPlayRate, IsBlend, fBlendRatio, IsLoop);
	}
}

_bool CBoxer::Play_Animation(_float fTimeDelta)
{
	_bool isAnimFinished = { false };

	isAnimFinished = dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Upper")))->Play_Animation(fTimeDelta);

	dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Face")))->Play_Animation(fTimeDelta);
	dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Head")))->Play_Animation(fTimeDelta);
	dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Weapon_R")))->Play_Animation(fTimeDelta);
	dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Weapon_L")))->Play_Animation(fTimeDelta);

	return isAnimFinished;
}

void CBoxer::Set_Collider_Active(const _wstring& strColliderTag, _bool bFlag)
{
	static_cast<CCollider*>(Find_Component(strColliderTag))->Set_Active(bFlag);
}

CCollider* CBoxer::Get_Collider(const _wstring& strColliderTag)
{
	return static_cast<CCollider*>(Find_Component(strColliderTag));
}

void CBoxer::OnCollision(COLLIDER_HANDLE_ID eHandleID)
{
	if (true == m_IsInvincible || true == m_IsPlayingDeadAnim)
		return;

	m_pGameManager->Active_Combo();

	CBoxerState* pNextState = { nullptr };

	_float fHitTime = { 0.3f };

	_vector vDirection = m_pTransformCom->Get_State(STATE::POSITION) -
		CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform()->Get_State(STATE::POSITION);

	if (COLLIDER_HANDLE_ID::PLAYER_HAND_ATTACK == eHandleID)
	{
		m_fCurrentHP -= 1.f;

		pNextState = CBoxer_BeatenState::Create(m_pNavigationCom, this, vDirection, 1.5f);
	}
	else if (COLLIDER_HANDLE_ID::PLAYER_HAND_ATTACK_FINAL == eHandleID)
	{
		m_fCurrentHP -= 3.f;
		pNextState = CBoxer_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection, 1.f);
	}
	else if (COLLIDER_HANDLE_ID::PLAYER_SWORD_ATTACK == eHandleID)
	{
		m_fCurrentHP -= 3.f;
		pNextState = CBoxer_BeatenState::Create(m_pNavigationCom, this, vDirection, 1.75f);
	}

	else if (COLLIDER_HANDLE_ID::PLAYER_SWORD_ATTACK_FINAL == eHandleID)
	{
		m_fCurrentHP -= 3.f;
		pNextState = CBoxer_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection, 1.f);
	}

	else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENGAN == eHandleID ||
		COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_FIREBALL == eHandleID)
	{
		m_fCurrentHP -= 15.f;
		pNextState = CBoxer_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection, 1.f);
	}

	else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENSHURIKEN == eHandleID ||
		COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENSHURIKEN_EXPLODE == eHandleID)
	{
		m_fCurrentHP -= 1.f;
		pNextState = CBoxer_BeatenState::Create(m_pNavigationCom, this, vDirection, 0.f);
	}

	else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_KAMUI == eHandleID)
	{
		m_fCurrentHP -= 1.f;
		pNextState = CBoxer_BeatenState::Create(m_pNavigationCom, this, vDirection, 0.f);
	}

	else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_KAMUI_END == eHandleID ||
		COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENSHURIKEN_END == eHandleID)
	{
		m_fCurrentHP -= 5.f;
		pNextState = CBoxer_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection, 1.f);
	}

	else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_BIGSHARK == eHandleID)
	{
		m_fCurrentHP -= 15.f;
		pNextState = CBoxer_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection, 1.f);
		Set_Invincible(1.f);
	}

	else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_CHIDORI == eHandleID)
	{
		m_pGameManager->Change_Camera(static_cast<LEVEL>(m_pGameInstance->Get_LevelID()),
			TEXT("Chidori_Action_Camera"), nullptr);

		fHitTime = 2.f;
		m_fCurrentHP -= 15.f;
		pNextState = CBoxer_ElectricShockState::Create(m_pNavigationCom, this);
		Set_Invincible(1.f);
	}

	if (m_fCurrentHP <= 0.f && false == m_IsPlayingDeadAnim)
	{
		m_IsPlayingDeadAnim = true;

		if (nullptr != pNextState)
		{
			pNextState->End();
			Safe_Release(pNextState);
		}

		pNextState = CBoxer_DeadState::Create(m_pNavigationCom, this);
	}

	for (auto& iter : m_PartObjects)
		static_cast<CParts_Character*>(iter.second)->Set_HitEffect(fHitTime, 1.f);

	Change_State(pNextState, false);

	m_fCurrentHP -= 10.f;
}

void CBoxer::Change_State(CBoxerState* pNextState, _bool bBlend)
{
	_bool IsBlend = m_pState->End();
	Safe_Release(m_pState);

	pNextState->Start(bBlend);
	m_pState = pNextState;
}

HRESULT CBoxer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoxer::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC	Desc{};
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fSpeedPerSec = 10.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Position()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_HPBar()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	m_pGameManager->Add_TargetTransform(m_pTransformCom);

	/* 상태 초기화 및 시작. */
	m_pState = CBoxer_IdleState::Create(m_pNavigationCom, this);
	m_pState->Start(true);

	m_fCurrentHP = 200.f;
	m_fMaxHP = 200.f;

	Fade_Particle();

	CTrail::TRAIL_DESC TrailDesc;
	TrailDesc.vFootTrailColor = _float4(0.3f, 1.0f, 0.3f, 1.f);
	XMStoreFloat4(&TrailDesc.vHighPosition, XMVectorSet(-0.02f, 0.f, 0.f, 1.f));
	XMStoreFloat4(&TrailDesc.vLowPosition, XMVectorSet(0.f, 0.f, 0.02f, 1.f));
	TrailDesc.strTrailTextureTag = TEXT("Prototype_Component_Texture_FootTrail_Blue");

	m_pFootTrail[0] = static_cast<CTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_FootTrail"), &TrailDesc));
	m_pFootTrail[1] = static_cast<CTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_FootTrail"), &TrailDesc));

	return S_OK;
}

void CBoxer::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CBoxer::Update(_float fTimeDelta)
{
	Update_FootTrail(fTimeDelta);
	Update_State(fTimeDelta);
	Update_SkillCoolDown(fTimeDelta);
	m_pNavigationCom->Compute_Height(m_pTransformCom);

	__super::Update(fTimeDelta);

	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	m_pLeafHurricaneColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	_matrix PlayerMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	_vector PlayerTranslation = m_pTransformCom->Get_State(STATE::POSITION);
	PlayerTranslation += m_pTransformCom->Get_State(STATE::LOOK) * 0.4f;
	PlayerMatrix.r[3] = PlayerTranslation;

	m_pHandAttackColliderCom->Update(PlayerMatrix);
	m_pSpinKickColliderCom->Update(PlayerMatrix);
}

void CBoxer::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	/* 파자마의 몸통 콜라이더를 콜리전 매니저에 등록 */
	m_pGameManager->Add_Object_ToCollision(TEXT("Monster_Body"), this, m_pColliderCom);
	m_pGameManager->Add_Collider_ToCollision(TEXT("Monster_Attack"), COLLIDER_HANDLE_ID::ENEMY_JETSU_ATTACK, m_pHandAttackColliderCom);
	m_pGameManager->Add_Collider_ToCollision(TEXT("Monster_Skill"), COLLIDER_HANDLE_ID::ENEMY_BOXER_LEAFHURRICANE, m_pLeafHurricaneColliderCom);
	m_pGameManager->Add_Collider_ToCollision(TEXT("Monster_Skill"), COLLIDER_HANDLE_ID::ENEMY_BOXER_SPINKICK, m_pSpinKickColliderCom);

	m_pNavigationCom->Compute_Height(m_pTransformCom);

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	LateUpdate_FootTrail(fTimeDelta);
#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->Add_DebugComponent(m_pHandAttackColliderCom);
	m_pGameInstance->Add_DebugComponent(m_pLeafHurricaneColliderCom);
	m_pGameInstance->Add_DebugComponent(m_pSpinKickColliderCom);
#endif
}

HRESULT CBoxer::Render()
{
	return S_OK;
}

_bool CBoxer::Use_Skill()
{
	if (m_fSkillTimeAcc >= m_fMaxSkillCoolDown)
	{
		m_fSkillTimeAcc = 0.f;
		return true;
	}
	else
		return false;
}

_bool CBoxer::Use_SpinKick()
{
	if (m_fSpinKickTimeAcc >= m_fMaxSpinKickCoolDown)
	{
		m_fSpinKickTimeAcc = 0.f;
		return true;
	}
	else
		return false;
}

void CBoxer::Update_SkillCoolDown(_float fTimeDelta)
{
	m_fSkillTimeAcc += fTimeDelta;
	m_fSpinKickTimeAcc += fTimeDelta;

	if (m_fSkillTimeAcc >= m_fMaxSkillCoolDown)
		m_fSkillTimeAcc = m_fMaxSkillCoolDown;

	if (m_fSpinKickTimeAcc >= m_fMaxSpinKickCoolDown)
		m_fSpinKickTimeAcc = m_fMaxSpinKickCoolDown;
}

HRESULT CBoxer::Ready_Components()
{
	/* Com_Navigation */
	CNavigation::NAVIGATION_DESC Desc;
	Desc.iCurrentCellIndex = 0;
	XMStoreFloat3(&Desc.vPosition, m_pTransformCom->Get_State(STATE::POSITION));

	if (LEVEL::TUTORIAL == m_pGameManager->Get_NextLevel())
	{
		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_Component_Navigation_Tutorial"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
			return E_FAIL;
	}
	else if (LEVEL::KONOHA_VILLAGE == m_pGameManager->Get_NextLevel())
	{
		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Navigation_KonohaVillage_3"),
			TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
			return E_FAIL;
	}

	/* Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC OBBDesc{};

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vSize = _float3(0.7f, 1.4f, 0.7f);
	OBBDesc.vCenter = _float3(0.f, 0.8f, 0.f);
	OBBDesc.isActive = true;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
		return E_FAIL;

	CBounding_Sphere::BOUNDING_SPHERE_DESC ColliderDesc{};

	ColliderDesc.fRadius = 0.7f;
	ColliderDesc.vCenter = _float3{ 0.f, 0.7f, 0.f };
	ColliderDesc.isActive = false;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_HandAttack"), reinterpret_cast<CComponent**>(&m_pHandAttackColliderCom), &ColliderDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_SpinKick"), reinterpret_cast<CComponent**>(&m_pSpinKickColliderCom), &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.fRadius = 1.5f;
	ColliderDesc.vCenter = _float3{ 0.f, 0.5f, 0.f };

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_LeafHurricane"), reinterpret_cast<CComponent**>(&m_pLeafHurricaneColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoxer::Ready_PartObjects()
{
	CUpper_Character::UPPER_PLAYER_DESC UpperDesc{};
	UpperDesc.pParentTransform = m_pTransformCom;
	UpperDesc.strModelName = TEXT("Prototype_Component_Model_Upper_Boxer");

	CHead_Character::HEAD_PLAYER_DESC HeadDesc{};
	HeadDesc.pParentTransform = m_pTransformCom;
	HeadDesc.strModelName = TEXT("Prototype_Component_Model_Head_Boxer");

	CFace_Character::tagFace_Player_Desc FaceDesc{};
	FaceDesc.pParentTransform = m_pTransformCom;
	FaceDesc.strModelName = TEXT("Prototype_Component_Model_Face_Boxer");

	/* Part_Upper */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Upper_Player"),
		TEXT("Part_Upper"), &UpperDesc)))
		return E_FAIL;

	/* Part_Head */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Head_Player"),
		TEXT("Part_Head"), &HeadDesc)))
		return E_FAIL;

	/* Part_Face */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Face_Player"),
		TEXT("Part_Face"), &FaceDesc)))
		return E_FAIL;

	CWeapon_Character::WEAPON_PLAYER_DESC WeaponDesc{};
	CUpper_Character* pUpperPlayer = dynamic_cast<CUpper_Character*>(Find_PartObject(TEXT("Part_Upper")));
	WeaponDesc.pParentTransform = m_pTransformCom;
	WeaponDesc.pUpper_Player = pUpperPlayer;
	WeaponDesc.pAttachMatrix = pUpperPlayer->Get_BoneMatrixPtr("LeftHand");
	WeaponDesc.strModelName = TEXT("Prototype_Component_Model_Weapon_Boxer_L");
	WeaponDesc.eType = CWeapon_Character::WEAPON_TYPE::GLOVE;
	/* Part_Weapon_R */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Weapon_Player"),
		TEXT("Part_Weapon_L"), &WeaponDesc)))
		return E_FAIL;

	WeaponDesc.pAttachMatrix = pUpperPlayer->Get_BoneMatrixPtr("RightHand");
	WeaponDesc.strModelName = TEXT("Prototype_Component_Model_Weapon_Boxer_R");
	WeaponDesc.eType = CWeapon_Character::WEAPON_TYPE::GLOVE;

	/* Part_Weapon_L */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Weapon_Player"),
		TEXT("Part_Weapon_R"), &WeaponDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoxer::Ready_Position()
{
	if (TRIGGER_TYPE::TUTORIAL_SPAWNER_01 == m_pGameManager->Get_CurrentTrigger())
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	else if (TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_02 == m_pGameManager->Get_CurrentTrigger())
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(67.787f, 12.7f, 37.657f, 1.f));

	return S_OK;
}


void CBoxer::Update_State(_float fTimeDelta)
{
	CBoxerState* pNextState = { nullptr };
	pNextState = m_pState->Update(fTimeDelta);

	if (nullptr != pNextState)
	{
		_bool IsBlend = m_pState->End();
		//현재 스테이트 날려버림.
		Safe_Release(m_pState);

		//새로운 상태 시작해줌. (내부적으로 복서 들게 됨. 레퍼런스 카운트 증가 안함.)
		pNextState->Start(IsBlend);

		m_pState = pNextState;
	}
}

CBoxer* CBoxer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CBoxer* pInstance = new CBoxer(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : Boxer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoxer::Clone(void* pArg)
{
	CBoxer* pInstance = new CBoxer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone  Failed : Boxer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoxer::Free()
{
	__super::Free();

	Safe_Release(m_pState);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pHandAttackColliderCom);
	Safe_Release(m_pLeafHurricaneColliderCom);
	Safe_Release(m_pSpinKickColliderCom);
	Safe_Release(m_pFootTrail[0]);
	Safe_Release(m_pFootTrail[1]);
}
