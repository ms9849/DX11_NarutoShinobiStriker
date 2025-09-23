#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "SkillSlotPanel.h"
#include "AttackTypePanel.h"
#include "ComboKOPanel.h"

#include "Head_Character.h"
#include "Face_Character.h"
#include "Upper_Character.h"
#include "Lower_Character.h"
#include "Weapon_Character.h"

#include "PlayerState.h"
#include "Player_IdleState.h"
#include "Player_BeatenState.h"
#include "Player_BeatenBlastedState.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CCharacter { pDevice, pContext, eObjectID }
	, m_pGameManager{ CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CCharacter{ rhs }
	, m_pGameManager{ CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}

const _float4x4* CPlayer::Get_BoneMatrix(const _wstring strPartTag, const _char* pBoneName)
{
	return static_cast<CParts_Character*>(Find_PartObject(strPartTag))->Get_BoneMatrixPtr(pBoneName);
}

CCollider* CPlayer::Get_WeaponCollider()
{
	return dynamic_cast<CWeapon_Character*>(Find_PartObject(TEXT("Part_Weapon")))->Get_Collider();
}

void CPlayer::Set_WeaponCollider_Active(_bool bFlag)
{
	CWeapon_Character* pWeapon = dynamic_cast<CWeapon_Character*>(Find_PartObject(TEXT("Part_Weapon")));
	pWeapon->Set_Collider_Active(bFlag);
}

_wstring CPlayer::Get_CurrentAnim()
{
	CParts_Character* pAnimParts = dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Upper")));

	return pAnimParts->Get_CurrentAnim();
}

_float CPlayer::Get_AnimProgress()
{
	/* 애니메이션 Progress 받아오기 */
	CParts_Character* pAnimParts = dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Upper")));

	return pAnimParts->Get_AnimProgress();
}

void CPlayer::Set_AnimProgress(_float fProgress)
{
	for (auto& iter : m_PartObjects)
	{
		static_cast<CParts_Character*>(iter.second)->Set_AnimProgress(fProgress);
	}
}

void CPlayer::Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate, _bool IsBlend, _float fBlendRatio, _bool IsLoop)
{
	/*
	플레이어 애니메이션 바꿔주기. 상하체만 바뀐다.
	추후 한벌옷도 추가 예정.
	*/
	for (auto& iter : m_PartObjects)
	{
		static_cast<CParts_Character*>(iter.second)->Set_AnimIndex(pAnimName, fAnimationPlayRate, IsBlend, fBlendRatio, IsLoop);
	}
}

void CPlayer::Set_Collider_Active(const _wstring& strColliderTag, _bool bFlag)
{
	static_cast<CCollider*>(Find_Component(strColliderTag))->Set_Active(bFlag);
}

CCollider* CPlayer::Get_Collider(const _wstring& strColliderTag)
{
	return static_cast<CCollider*>(Find_Component(strColliderTag));
}

void CPlayer::Update_State(_float fTimeDelta)
{
	CPlayerState* pNextState = { nullptr };
	pNextState = m_pState->Update(fTimeDelta);

	if (nullptr != pNextState)
	{
		_bool IsBlend = m_pState->End();
		//현재 스테이트 날려버려.
		Safe_Release(m_pState);

		//새로운 상태 시작해줘. (내부적으로 플레이어 들게 됨)
		pNextState->Start(IsBlend);

		m_pState = pNextState;
	}
}

_bool CPlayer::Play_Animation(_float fTimeDelta)
{
	_bool isAnimFinished = { false };

	isAnimFinished = dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Upper")))->Play_Animation(fTimeDelta);

	dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Lower")))->Play_Animation(fTimeDelta);
	dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Face")))->Play_Animation(fTimeDelta);
	dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Head")))->Play_Animation(fTimeDelta);
	dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Weapon")))->Play_Animation(fTimeDelta);

	return isAnimFinished;
}

_bool CPlayer::Use_Skill(SKILLNUM eSlotNum)
{
	SKILL_INFO*  pInfo = Get_Skill_Info(Get_Skill_Slot(eSlotNum));

		/* 필살기라면 게이지 0, 일반 스킬이라면 쿨타임 돌게끔. */
	if (pInfo->IsSpecial == true && pInfo->fMaxGauge >= pInfo->fGaugeAcc)
	{
		pInfo->fGaugeAcc = 0.f;
		return true;

	}
	else if (pInfo->IsSpecial == false && pInfo->fTimeAcc >= pInfo->fMaxCoolDown)
	{
		pInfo->fTimeAcc = 0.f;
		return true;
	}

	return false;
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC	Desc{};
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fSpeedPerSec = 8.5f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	/* 게임 매니저에 현재 플레이어 정보 세팅. 레벨 변경되도 안전할거니까.. */
	m_pGameManager->Set_PlayerPtr(this);

	/* 플레이어는 모든 스킬을 사용할 수 있다. */
	for (_uint i = 0; i < ENUM_CLASS(SKILL::END); ++i)
		m_Skills.emplace(static_cast<SKILL>(i), g_SkillTable[i]);

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	/* 상태 초기화 및 시작. */
	m_pState = CPlayer_IdleState::Create(this);
	m_pState->Start(true);

	if (LEVEL::TUTORIAL == m_pGameManager->Get_NextLevel())
	{
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.3f, 0.031f, -51.82f, 1.f));
	}


	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
	m_pGameManager->Update_LockOnManager(fTimeDelta);

	/* 스테이트 업데이트. */
	if(false == m_pGameManager->IsTalking())
		Update_State(fTimeDelta);

	/* 네비메쉬 높이 업데이트 */

	if(true == m_IsGround)
		m_pNavigationCom->Compute_Height(m_pTransformCom);

	
	Key_Input(fTimeDelta);

	/* 쿨타임 계산 */
	for (auto& iter : m_Skills)
		iter.second.fTimeAcc += fTimeDelta;

	__super::Update(fTimeDelta);

	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

	_matrix PlayerMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
	_vector PlayerTranslation = m_pTransformCom->Get_State(STATE::POSITION);
	PlayerTranslation += m_pTransformCom->Get_State(STATE::LOOK) * 0.4f;
	PlayerMatrix.r[3] = PlayerTranslation;

	m_pHandAttackColliderCom->Update(PlayerMatrix);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	/* 스킬이 바뀌었다면. */
	if (m_eCurAttackType != m_ePreAttackType)
	{
		__super::Change_AttackType();
		Change_Skills();
	}

	if (false == m_IsVisible)
		return;

	__super::Late_Update(fTimeDelta);

	m_pGameManager->Add_Object_ToCollision(TEXT("Player_Body"), this, m_pColliderCom);
	m_pGameManager->Add_Collider_ToCollision(TEXT("Player_Body"), COLLIDER_HANDLE_ID::PLAYER_BODY, m_pColliderCom);
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CPlayer::Render()
{
#ifdef _DEBUG

	m_pColliderCom->Render();
	m_pHandAttackColliderCom->Render();

#endif

	return S_OK;
}

void CPlayer::OnCollision(COLLIDER_HANDLE_ID eHandleID, _float3 vColliderPos)
{
	/* 플레이어 충돌로직 처리 */
	if (true == m_IsInvincible || true == m_pGameManager->IsTalking())
		return;

	CPlayerState* pNextState = { nullptr };

	_vector vDirection = m_pTransformCom->Get_State(STATE::POSITION) - XMLoadFloat3(&vColliderPos);

	m_pTransformCom->LookAt_XZ(XMLoadFloat3(&vColliderPos));

	if (COLLIDER_HANDLE_ID::ENEMY_BIRD_THROW == eHandleID ||
		COLLIDER_HANDLE_ID::ENEMY_JETSU_ATTACK == eHandleID)
	{
		pNextState = CPlayer_BeatenState::Create(this, vDirection, 1.f);
	}
	else if (COLLIDER_HANDLE_ID::ENEMY_JETSU_WOODHAND == eHandleID)
	{
		pNextState = CPlayer_BeatenBlastedState::Create(this, vDirection, 1.f);
	}


	/* 플레이어 죽는 상태 X */
	Change_State(pNextState, false);
}

void CPlayer::Change_State(CPlayerState* pNextState, _bool bBlend)
{
	_bool IsBlend = m_pState->End();
	Safe_Release(m_pState);

	pNextState->Start(bBlend);
	m_pState = pNextState;
}

HRESULT CPlayer::Ready_Components()
{
	/* Com_Navigation */
	CNavigation::NAVIGATION_DESC Desc;
	Desc.iCurrentCellIndex = 0;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Test_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
		return E_FAIL;

	/* Com_Collider */

	CBounding_Sphere::BOUNDING_SPHERE_DESC ColliderDesc{};

	ColliderDesc.fRadius = 0.7f;
	ColliderDesc.vCenter = _float3{ 0.f, 0.7f, 0.f };
	ColliderDesc.isActive = true;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	
	ColliderDesc.isActive = false;
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_HandAttack"), reinterpret_cast<CComponent**>(&m_pHandAttackColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	CUpper_Character::UPPER_PLAYER_DESC UpperDesc{};
	UpperDesc.pParentTransform = m_pTransformCom;
	UpperDesc.strModelName = TEXT("Prototype_Component_Model_Upper_Player");

	CLower_Character::LOWER_PLAYER_DESC LowerDesc{};
	LowerDesc.pParentTransform = m_pTransformCom;
	LowerDesc.strModelName = TEXT("Prototype_Component_Model_Lower_Player");

	CHead_Character::HEAD_PLAYER_DESC HeadDesc{};
	HeadDesc.pParentTransform = m_pTransformCom;
	HeadDesc.strModelName = TEXT("Prototype_Component_Model_Head_Player");

	CFace_Character::tagFace_Player_Desc FaceDesc{};
	FaceDesc.pParentTransform = m_pTransformCom;
	FaceDesc.strModelName = TEXT("Prototype_Component_Model_Face_Player");

	/* Part_Upper */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Prototype_GameObject_Upper_Player"),
		TEXT("Part_Upper"), &UpperDesc)))
		return E_FAIL;

	/* Part_Lower */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Prototype_GameObject_Lower_Player"),
		TEXT("Part_Lower"), &LowerDesc)))
		return E_FAIL;

	/* Part_Head */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Prototype_GameObject_Head_Player"),
		TEXT("Part_Head"), &HeadDesc)))
		return E_FAIL;

	/* Part_Face */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Prototype_GameObject_Face_Player"),
		TEXT("Part_Face"), &FaceDesc)))
		return E_FAIL;

	CWeapon_Character::WEAPON_PLAYER_DESC WeaponDesc{};
	CUpper_Character* pUpperPlayer = dynamic_cast<CUpper_Character*>(Find_PartObject(TEXT("Part_Upper")));
	WeaponDesc.pParentTransform = m_pTransformCom;
	WeaponDesc.pAttachMatrix = pUpperPlayer->Get_BoneMatrixPtr("Attach_Sword");
	WeaponDesc.pHandMatrix = pUpperPlayer->Get_BoneMatrixPtr("R_Hand_Weapon_cnt_tr");
	WeaponDesc.pUpper_Player = pUpperPlayer; 
	WeaponDesc.strModelName = TEXT("Prototype_Component_Model_Weapon_Player");
	WeaponDesc.eType = CWeapon_Character::WEAPON_TYPE::SWORD;
	/* Part_Weapon */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Prototype_GameObject_Weapon_Player"),
		TEXT("Part_Weapon"), &WeaponDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Key_Input(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_6))
	{
		m_eCurAttackType = static_cast<ATTACK_TYPE>(ENUM_CLASS(m_eCurAttackType) + 1);

		if (m_eCurAttackType == ATTACK_TYPE::END)
			m_eCurAttackType = ATTACK_TYPE::MELEE;
	}

	//if (m_pGameInstance->Key_Down(DIK_7))
	//{
	//	/* 콤보 시스템에서 초기화해줌.*/
	//	/* 로직은 묶였지만 아직 bool 타입 못 묶음..*/
	//	m_IsEnemyHit = true;
	//}

	//if (m_pGameInstance->Key_Down(DIK_8))
	//{
	//	/* 콤보 시스템에서 초기화해줌.*/
	//	/* 로직은 묶였지만 아직 bool 타입 못 묶음..*/
	//	m_IsEnemyKO = true;
	//}
}

void CPlayer::Change_Skills()
{
	/* 첫 번째 스킬은 그림자 분신이니까 교체 안함. */
	for (_uint i = ENUM_CLASS(SKILLNUM::SECOND); i < ENUM_CLASS(SKILLNUM::END); ++i)
		m_pGameManager->Change_Skill(i, m_ActivatedSkills[i]);

	m_pGameManager->Change_AttackType(m_eCurAttackType);
}

void CPlayer::Clear_State()
{
	//순환참조 해결용.
	Safe_Release(m_pState);
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : PLAYER ");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : PLAYER ");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pState);
	Safe_Release(m_pGameManager);

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pHandAttackColliderCom);
	Safe_Release(m_pNavigationCom);
}
