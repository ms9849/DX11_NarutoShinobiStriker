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

	return S_OK;
}

void CBoxer::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CBoxer::Update(_float fTimeDelta)
{
	Update_State(fTimeDelta);
	Update_SkillCoolDown(fTimeDelta);
	m_pNavigationCom->Compute_Height(m_pTransformCom);

	__super::Update(fTimeDelta);

	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CBoxer::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CBoxer::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

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

void CBoxer::Update_SkillCoolDown(_float fTimeDelta)
{
	m_fSkillTimeAcc += fTimeDelta;

	if (m_fSkillTimeAcc >= m_fMaxSkillCoolDown)
		m_fSkillTimeAcc = m_fMaxSkillCoolDown;
}

HRESULT CBoxer::Ready_Components()
{
	/* Com_Navigation */

	//CNavigation::NAVIGATION_DESC Desc;
	//Desc.iCurrentCellIndex = 1;

	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_NavigationMesh"),
	//	TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
	//	return E_FAIL;

	/* Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC OBBDesc{};

	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vSize = _float3(0.7f, 1.4f, 0.7f);
	OBBDesc.vCenter = _float3(0.f, 0.8f, 0.f);
	OBBDesc.isActive = true;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
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
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Upper_Player"),
		TEXT("Part_Upper"), &UpperDesc)))
		return E_FAIL;

	/* Part_Head */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Head_Player"),
		TEXT("Part_Head"), &HeadDesc)))
		return E_FAIL;

	/* Part_Face */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Face_Player"),
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
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Weapon_Player"),
		TEXT("Part_Weapon_L"), &WeaponDesc)))
		return E_FAIL;

	WeaponDesc.pAttachMatrix = pUpperPlayer->Get_BoneMatrixPtr("RightHand");
	WeaponDesc.strModelName = TEXT("Prototype_Component_Model_Weapon_Boxer_R");
	WeaponDesc.eType = CWeapon_Character::WEAPON_TYPE::GLOVE;

	/* Part_Weapon_L */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Weapon_Player"),
		TEXT("Part_Weapon_R"), &WeaponDesc)))
		return E_FAIL;

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
}
