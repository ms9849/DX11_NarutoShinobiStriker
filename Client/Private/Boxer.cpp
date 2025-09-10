#include "Boxer.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Head_Character.h"
#include "Face_Character.h"
#include "Upper_Character.h"
#include "Lower_Character.h"
#include "Weapon_Character.h"

CBoxer::CBoxer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CEnemy { pDevice, pContext, eObjectID }
{
}

CBoxer::CBoxer(const CBoxer& rhs)
	: CEnemy{ rhs }
{
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

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(5.f, 0.f, 6.f, 1.f));
	m_pGameManager->Add_TargetTransform(m_pTransformCom);

	/* 상태 초기화 및 시작. */
	//m_pState = CWhiteJetsu_IdleState::Create(m_pTransformCom, m_pNavigationCom, m_pModelCom);
	//m_pState->Start(true);

	return S_OK;
}

void CBoxer::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CBoxer::Update(_float fTimeDelta)
{
	Update_State(fTimeDelta);

	__super::Update(fTimeDelta);
}

void CBoxer::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	Set_AnimIndex("CustomMan_Idle_Loop", 1.f, true, 0.1f);
	Play_Animation(fTimeDelta);
}

HRESULT CBoxer::Render()
{
	return S_OK;
}

HRESULT CBoxer::Ready_Components()
{
	/* Com_Navigation */

	CNavigation::NAVIGATION_DESC Desc;
	Desc.iCurrentCellIndex = 1;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Test_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
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
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Upper_Player"),
		TEXT("Part_Upper"), &UpperDesc)))
		return E_FAIL;

	/* Part_Head */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Head_Player"),
		TEXT("Part_Head"), &HeadDesc)))
		return E_FAIL;

	/* Part_Face */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Face_Player"),
		TEXT("Part_Face"), &FaceDesc)))
		return E_FAIL;

	CWeapon_Character::WEAPON_PLAYER_DESC WeaponDesc{};
	CUpper_Character* pUpperPlayer = dynamic_cast<CUpper_Character*>(Find_PartObject(TEXT("Part_Upper")));
	WeaponDesc.pParentTransform = m_pTransformCom;
	WeaponDesc.pUpper_Player = pUpperPlayer;
	WeaponDesc.pAttachMatrix = pUpperPlayer->Get_BoneMatrixPtr("LeftHand");
	WeaponDesc.strModelName = TEXT("Prototype_Component_Model_Weapon_Boxer_L");

	/* Part_Weapon_R */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Weapon_Player"),
		TEXT("Part_Weapon_L"), &WeaponDesc)))
		return E_FAIL;

	WeaponDesc.pAttachMatrix = pUpperPlayer->Get_BoneMatrixPtr("RightHand");
	WeaponDesc.strModelName = TEXT("Prototype_Component_Model_Weapon_Boxer_R");

	/* Part_Weapon_L */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Weapon_Player"),
		TEXT("Part_Weapon_R"), &WeaponDesc)))
		return E_FAIL;

	return S_OK;
}


void CBoxer::Update_State(_float fTimeDelta)
{
	//CPlayerState* pNextState = { nullptr };
	//pNextState = m_pState->Update(fTimeDelta);

	//if (nullptr != pNextState)
	//{
	//	_bool IsBlend = m_pState->End();
	//	//현재 스테이트 날려버려.
	//	Safe_Release(m_pState);

	//	//새로운 상태 시작해줘. (내부적으로 플레이어 들게 됨)
	//	pNextState->Start(IsBlend);

	//	m_pState = pNextState;
	//}
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

	Safe_Release(m_pNavigationCom);
}
