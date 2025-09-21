#include "GameManager.h"

#include "GameInstance.h"

#include "Player.h"
#include "QuestLog.h"
#include "Enemy.h"

#include "Camera_Manager.h"
#include "LockOn_Manager.h"
#include "Collision_Manager.h"
#include "UI_Manager.h"

IMPLEMENT_SINGLETON(CGameManager);

CGameManager::CGameManager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CGameManager::Initialize_GameManager()
{
	m_pCamera_Manager = CCamera_Manager::Create();
	if (nullptr == m_pCamera_Manager)
		return E_FAIL;

	m_pCollision_Manager = CCollision_Manager::Create();
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;

	m_pLockOn_Manager = CLockOn_Manager::Create();
	if (nullptr == m_pLockOn_Manager)
		return E_FAIL;

	m_pUI_Manager = CUI_Manager::Create();
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return S_OK;
}

void CGameManager::Release_GameManager()
{
	DestroyInstance();

	if(nullptr != m_pPlayer)
		m_pPlayer->Clear_State();

	Safe_Release(m_pPlayer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pCamera_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pLockOn_Manager);
	Safe_Release(m_pUI_Manager);
}

void CGameManager::Clear()
{
	m_pCamera_Manager->Clear();
	m_pCollision_Manager->Clear();


	/* 지울때 레퍼런스 카운트가 0임을 보장하지 않음*/
	/* 따라서 명시적으로 nullptr 처리 해줘야함 */

	/* 플레이어 지우기 */
	Safe_Release(m_pPlayer);
	if (nullptr != m_pPlayer)
	{
		m_pPlayer->Clear_State();
		m_pPlayer = nullptr;
	}
}

CTransform* CGameManager::Get_TargetTransform()
{
	return m_pCamera_Manager->Get_TargetTransform();
}

void CGameManager::SetUp_Target()
{
	return m_pCamera_Manager->SetUp_Target();
}

HRESULT CGameManager::Add_TargetTransform(CTransform* pTransformCom)
{
 	return m_pCamera_Manager->Add_TargetTransform(pTransformCom);
}

HRESULT CGameManager::Set_PlayerPtr(CPlayer* pPlayer)
{
	//인자로 받은 플레이어가 nullptr 이라면
	if (nullptr == pPlayer)
		return E_FAIL;

	//이미 플레이어가 등록되어 있다면,
	if (nullptr != m_pPlayer)
		Safe_Release(m_pPlayer);

	m_pPlayer = pPlayer;
	Safe_AddRef(m_pPlayer);

	return S_OK;
}

HRESULT CGameManager::Set_NextLevelID(LEVEL eLevelID)
{
	m_eNextLevel = eLevelID;

	return S_OK;
}

HRESULT CGameManager::Add_Camera(LEVEL eLevelID, const _wstring& strCameraTag, CCamera* pCamera)
{
    return m_pCamera_Manager->Add_Camera(eLevelID, strCameraTag, pCamera);
}

HRESULT CGameManager::Change_Camera(LEVEL eLevelID, const _wstring& strCameraTag)
{
	return m_pCamera_Manager->Change_Camera(eLevelID, strCameraTag);
}

void CGameManager::Add_Collider_ToCollision(const _wstring& strColliderTag, COLLIDER_HANDLE_ID eHandleID, CCollider* pCollider)
{
	m_pCollision_Manager->Add_Collider_ToCollision(strColliderTag, eHandleID, pCollider);
}

void CGameManager::Add_Object_ToCollision(const _wstring& strObjectTag, CGameObject* pGameObject, CCollider* pCollider)
{
	m_pCollision_Manager->Add_Object_ToCollision(strObjectTag, pGameObject, pCollider);
}

void CGameManager::Update_Collision()
{
	m_pCollision_Manager->Update();
}

void CGameManager::Check_Collision(const _wstring strColliderTag, const _wstring strObjectTag, COLLISION_TYPE eColType)
{
	m_pCollision_Manager->Check_Collision(strColliderTag, strObjectTag, eColType);
}

void CGameManager::Update_LockOnManager(_float fTimeDelta)
{
	m_pLockOn_Manager->Update(fTimeDelta);
}

CTransform* CGameManager::Calc_Target(_fvector vPosition)
{
	return m_pLockOn_Manager->Calc_Target(vPosition);
}

void CGameManager::Set_Dialog(CDialogUI* pDialogUI)
{
	m_pUI_Manager->Set_Dialog(pDialogUI);
}

void CGameManager::Set_SkillSlotPanel(CSkillSlotPanel* pSkillSlotPanel)
{
	m_pUI_Manager->Set_SkillSlotPanel(pSkillSlotPanel);
}

void CGameManager::Set_AttackTypePanel(CAttackTypePanel* pAttackTypePanel)
{
	m_pUI_Manager->Set_AttackTypePanel(pAttackTypePanel);
}

void CGameManager::Set_ComboKoPanel(CComboKOPanel* pComboKOPanel)
{
	m_pUI_Manager->Set_ComboKoPanel(pComboKOPanel);
}

void CGameManager::Set_Dialog_Text(const _wstring& strDialogText)
{
	m_pUI_Manager->Set_Dialog_Text(strDialogText);
}

void CGameManager::Set_Dialog_Visible(_bool bFlag)
{
	m_pUI_Manager->Set_Dialog_Visible(bFlag);
}

void CGameManager::Update_Combo(_uint iComboCount)
{
	m_pUI_Manager->Update_Combo(iComboCount);
}

void CGameManager::PopUp_KO()
{
	m_pUI_Manager->PopUp_KO();
}

void CGameManager::Change_Skill(_uint iIdx, SKILL eSkill)
{
	m_pUI_Manager->Change_Skill(iIdx, eSkill);
}

void CGameManager::Change_AttackType(ATTACK_TYPE eAttackType)
{
	m_pUI_Manager->Change_AttackType(eAttackType);
}

LEVEL CGameManager::Get_NextLevel()
{
	if (m_pGameInstance->Get_LevelID() != ENUM_CLASS(LEVEL::LOADING))
		return LEVEL::END;

	return m_eNextLevel;
}

void CGameManager::Free()
{
	__super::Free();
}
