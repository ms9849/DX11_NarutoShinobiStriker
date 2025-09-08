#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "SkillSlotPanel.h"
#include "AttackTypePanel.h"
#include "ComboKOPanel.h"

#include "Head_Player.h"
#include "Face_Player.h"
#include "Upper_Player.h"
#include "Lower_Player.h"
#include "Weapon_Player.h"

#include "PlayerState.h"
#include "Player_IdleState.h"

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

_float CPlayer::Get_AnimProgress()
{
	/* 애니메이션 Progress 받아오기 */
	CParts_Player* pAnimParts = dynamic_cast<CParts_Player*>(Find_PartObject(TEXT("Part_Upper")));
	if (nullptr != pAnimParts)
		return pAnimParts->Get_AnimProgress();
}

void CPlayer::Set_AnimProgress(_float fProgress)
{
	for (auto& iter : m_PartObjects)
	{
		static_cast<CParts_Player*>(iter.second)->Set_AnimProgress(fProgress);
	}
}

#pragma region UI_Initialize

void CPlayer::Set_SkillSlotPanel(CSkillSlotPanel* pPanel)
{
	m_pSkillSlotPanel = pPanel;
	Safe_AddRef(m_pSkillSlotPanel);

	for (_uint i = ENUM_CLASS(SKILLNUM::SECOND); i < ENUM_CLASS(SKILLNUM::END); ++i)
		m_pSkillSlotPanel->Change_Skill(i, m_ActivatedSkills[i]);
}

void CPlayer::Set_AttackTypePanel(CAttackTypePanel* pPanel)
{
	m_pAttackTypePanel = pPanel;
	Safe_AddRef(m_pAttackTypePanel);

	m_pAttackTypePanel->Change_AttackType(m_eCurAttackType);
}

void CPlayer::Set_ComboKOPanel(CComboKOPanel* pPanel)
{
	m_pComboKOPanel = pPanel;
	Safe_AddRef(m_pComboKOPanel);
}

#pragma endregion

void CPlayer::Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate, _bool IsBlend, _float fBlendRatio, _bool IsLoop)
{
	/*
	플레이어 애니메이션 바꿔주기. 상하체만 바뀐다.
	추후 한벌옷도 추가 예정.
	*/
	for (auto& iter : m_PartObjects)
	{
		static_cast<CParts_Player*>(iter.second)->Set_AnimIndex(pAnimName, fAnimationPlayRate, IsBlend, fBlendRatio, IsLoop);
	}
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

	isAnimFinished = dynamic_cast<CParts_Player*>(Find_PartObject(TEXT("Part_Upper")))->Play_Animation(fTimeDelta);

	dynamic_cast<CParts_Player*>(Find_PartObject(TEXT("Part_Lower")))->Play_Animation(fTimeDelta);
	dynamic_cast<CParts_Player*>(Find_PartObject(TEXT("Part_Face")))->Play_Animation(fTimeDelta);
	dynamic_cast<CParts_Player*>(Find_PartObject(TEXT("Part_Head")))->Play_Animation(fTimeDelta);
	dynamic_cast<CParts_Player*>(Find_PartObject(TEXT("Part_Weapon")))->Play_Animation(fTimeDelta);

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
	Desc.fSpeedPerSec = 10.f;

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

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
	/* 스테이트 업데이트. */
	Update_State(fTimeDelta);
	Key_Input(fTimeDelta);
	ComboKO_System(fTimeDelta);

	/* 쿨타임 계산 */
	for (auto& iter : m_Skills)
		iter.second.fTimeAcc += fTimeDelta;

	__super::Update(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	/* 스킬이 바뀌었다면. */
	if (m_eCurAttackType != m_ePreAttackType)
	{
		__super::Change_AttackType();
		Change_Skills();
	}

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

	__super::Late_Update(fTimeDelta);
}

HRESULT CPlayer::Render()
{
	if(FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	CUpper_Player::UPPER_PLAYER_DESC UpperDesc{};
	UpperDesc.pParentTransform = m_pTransformCom;

	CLower_Player::LOWER_PLAYER_DESC LowerDesc{};
	LowerDesc.pParentTransform = m_pTransformCom;

	CHead_Player::HEAD_PLAYER_DESC HeadDesc{};
	HeadDesc.pParentTransform = m_pTransformCom;

	CFace_Player::tagFace_Player_Desc FaceDesc{};
	FaceDesc.pParentTransform = m_pTransformCom;

	/* Part_Upper */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Upper_Player"),
		TEXT("Part_Upper"), &UpperDesc)))
		return E_FAIL;

	/* Part_Lower */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Lower_Player"),
		TEXT("Part_Lower"), &LowerDesc)))
		return E_FAIL;

	/* Part_Head */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Head_Player"),
		TEXT("Part_Head"), &HeadDesc)))
		return E_FAIL;

	/* Part_Face */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Face_Player"),
		TEXT("Part_Face"), &FaceDesc)))
		return E_FAIL;

	CWeapon_Player::WEAPON_PLAYER_DESC WeaponDesc{};
	CUpper_Player* pUpperPlayer = dynamic_cast<CUpper_Player*>(Find_PartObject(TEXT("Part_Upper")));
	WeaponDesc.pParentTransform = m_pTransformCom;
	WeaponDesc.pAttachMatrix = pUpperPlayer->Get_BoneMatrixPtr("Attach_Sword");
	WeaponDesc.pHandMatrix = pUpperPlayer->Get_BoneMatrixPtr("R_Hand_Weapon_cnt_tr");
	WeaponDesc.pUpper_Player = pUpperPlayer; 

	/* Part_Weapon */
	if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Weapon_Player"),
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

	if (m_pGameInstance->Key_Down(DIK_7))
	{
		/* 콤보 시스템에서 초기화해줌.*/
		/* 로직은 묶였지만 아직 bool 타입 못 묶음..*/
		m_bEnemyHit = true;
	}

	if (m_pGameInstance->Key_Down(DIK_8))
	{
		/* 콤보 시스템에서 초기화해줌.*/
		/* 로직은 묶였지만 아직 bool 타입 못 묶음..*/
		m_bEnemyKO = true;
	}
}

void CPlayer::ComboKO_System(_float fTimeDelta)
{
	m_fComboTimeAcc += fTimeDelta;

	/* KO 시스템 */
	if (m_bEnemyKO)
	{
		m_pComboKOPanel->PopUp_KO();
		m_bEnemyKO = false;
	}

	/* 콤보 시스템 */
	if (m_bEnemyHit)
	{
		if(m_iComboCount < m_iMaxComboCount)
			m_iComboCount++;
		m_fComboTimeAcc = 0.f;
	}

	/* 콤보 초기화 */
	if (m_iComboCount >= 1 && m_fComboTimeAcc >= 2.f)
	{
		m_iComboCount = 0;
		m_fComboTimeAcc = 0.f;
	}

	m_pComboKOPanel->Update_Combo(m_iComboCount);
	m_bEnemyHit = false;
}

void CPlayer::Change_Skills()
{
	if (m_pSkillSlotPanel == nullptr || m_pAttackTypePanel == nullptr)
		return;

	/* 첫 번째 스킬은 그림자 분신이니까 교체 안함. */
	for (_uint i = ENUM_CLASS(SKILLNUM::SECOND); i < ENUM_CLASS(SKILLNUM::END); ++i)
		m_pSkillSlotPanel->Change_Skill(i, m_ActivatedSkills[i]);

	m_pAttackTypePanel->Change_AttackType(m_eCurAttackType);
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
	Safe_Release(m_pSkillSlotPanel);
	Safe_Release(m_pAttackTypePanel);
	Safe_Release(m_pComboKOPanel);
}
