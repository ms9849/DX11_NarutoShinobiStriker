#include "GameManager.h"

#include "GameInstance.h"

#include "Player.h"
#include "QuestLog.h"
#include "Enemy.h"

#include "Camera_Manager.h"
#include "LockOn_Manager.h"
#include "Collision_Manager.h"
#include "UI_Manager.h"
#include "Trigger_Manager.h"
#include "Player_Manager.h"

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

	m_pTrigger_Manager = CTrigger_Manager::Create();
	if (nullptr == m_pTrigger_Manager)
		return E_FAIL;

	m_pPlayer_Manager = CPlayer_Manager::Create();
	if (nullptr == m_pPlayer_Manager)
		return E_FAIL;

	return S_OK;
}

void CGameManager::Release_GameManager()
{
	DestroyInstance();

	Safe_Release(m_pPlayer_Manager);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pCamera_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pLockOn_Manager);
	Safe_Release(m_pUI_Manager);
	Safe_Release(m_pTrigger_Manager);
}

void CGameManager::Clear()
{
	m_pCamera_Manager->Clear();
	m_pCollision_Manager->Clear();
	m_pPlayer_Manager->Clear();
}

_wstring CGameManager::Get_CameraName()
{
	return m_pCamera_Manager->Get_CameraName();
}

HRESULT CGameManager::Add_TargetTransform(CTransform* pTransformCom)
{
 	return m_pCamera_Manager->Add_TargetTransform(pTransformCom);
}

#pragma region PLAYER
CPlayer* CGameManager::Get_PlayerPtr()
{
	return m_pPlayer_Manager->Get_PlayerPtr();
}

HRESULT CGameManager::Set_PlayerPtr(CPlayer* pPlayer)
{
	return m_pPlayer_Manager->Set_PlayerPtr(pPlayer);
}

vector<pair<_wstring, _wstring>>& CGameManager::Get_Outfits(SELECT_TYPE eType)
{
	return m_pPlayer_Manager->Get_Outfits(eType);
}

void CGameManager::Add_Outfit(SELECT_TYPE eType, const _wstring& strOutfitTag, const _wstring& strModelTag)
{
	return m_pPlayer_Manager->Add_Outfit(eType, strOutfitTag, strModelTag);
}
void CGameManager::Set_PlayerModelInfo(SELECT_TYPE eType, _uint iModelNum)
{
	m_pPlayer_Manager->Set_PlayerModelInfo(eType, iModelNum);
}

_wstring CGameManager::Get_PlayerModelInfo(SELECT_TYPE eType)
{
	return m_pPlayer_Manager->Get_PlayerModelInfo(eType);
}

_bool CGameManager::IsOneCloth()
{
	return m_pPlayer_Manager->IsOneCloth();
}

#pragma endregion

HRESULT CGameManager::Set_NextLevelID(LEVEL eLevelID)
{
	m_eNextLevel = eLevelID;
	return S_OK;
}

HRESULT CGameManager::Add_Camera(LEVEL eLevelID, const _wstring& strCameraTag, CCamera* pCamera)
{
    return m_pCamera_Manager->Add_Camera(eLevelID, strCameraTag, pCamera);
}

HRESULT CGameManager::Change_Camera(LEVEL eLevelID, const _wstring& strCameraTag,  const _float4x4* pWorldMatrix)
{
	return m_pCamera_Manager->Change_Camera(eLevelID, strCameraTag, pWorldMatrix);
}

void CGameManager::Shake_Camera(_float fShakeTime, _float fIntensity)
{
	m_pCamera_Manager->Shake(fShakeTime, fIntensity);
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

void CGameManager::Set_MissionAlertPanel(CMissionAlertPanel* pMissionAlertPanel)
{
	m_pUI_Manager->Set_MissionAlertPanel(pMissionAlertPanel);
}

void CGameManager::Set_WinPanel(CWinPanel* pWinPanel)
{
	m_pUI_Manager->Set_WinPanel(pWinPanel);
}

void CGameManager::Set_CutScenePanel(CCutScenePanel* pCutScenePanel)
{
	m_pUI_Manager->Set_CutScenePanel(pCutScenePanel);
}

void CGameManager::Set_Dialog_Text(const _wstring& strDialogText)
{
	m_pUI_Manager->Set_Dialog_Text(strDialogText);
}

void CGameManager::Set_Dialog_Visible(_bool bFlag)
{
	m_pUI_Manager->Set_Dialog_Visible(bFlag);
}

void CGameManager::Active_Combo()
{
	m_pUI_Manager->Active_Combo();
}

void CGameManager::Active_KO()
{
	m_pUI_Manager->Active_KO();
}

void CGameManager::Change_Skill(_uint iIdx, SKILL eSkill)
{
	m_pUI_Manager->Change_Skill(iIdx, eSkill);
}

void CGameManager::Set_SkillSlot_Visible(_bool bFlag)
{
	m_pUI_Manager->Set_SkillSlot_Visible(bFlag); 
}

void CGameManager::Change_AttackType(ATTACK_TYPE eAttackType)
{
	m_pUI_Manager->Change_AttackType(eAttackType);
}

void CGameManager::Set_HpProgress(_float fProgress)
{
	m_pUI_Manager->Set_HpProgress(fProgress);
}

void CGameManager::Set_MaxHpProgress(_float fMaxProgress)
{
	m_pUI_Manager->Set_MaxHpProgress(fMaxProgress);
}

void CGameManager::Set_AttackType_Visible(_bool bFlag)
{
	m_pUI_Manager->Set_AttackType_Visible(bFlag);
}

void CGameManager::AlertPanel_Start_FadeIn(const _wstring& strMissionText)
{
	m_pUI_Manager->AlertPanel_Start_FadeIn(strMissionText);
}

void CGameManager::WinPanel_Start_FadeIn()
{
	m_pUI_Manager->WinPanel_Start_FadeIn();
}

void CGameManager::Set_CutScene_Visible(_bool bFlag)
{
	m_pUI_Manager->Set_CutScene_Visible(bFlag);
}

HRESULT CGameManager::OnTrigger(TRIGGER_TYPE eTriggerType)
{
	return m_pTrigger_Manager->OnTrigger(eTriggerType);
}

TRIGGER_TYPE CGameManager::Get_CurrentTrigger()
{
	return m_pTrigger_Manager->Get_CurrentTrigger();
}

_bool CGameManager::IsActivated(TRIGGER_TYPE eTriggerType)
{
	return m_pTrigger_Manager->IsActivated(eTriggerType);
}

LEVEL CGameManager::Get_NextLevel()
{
	return m_eNextLevel;
}

void CGameManager::Free()
{
	__super::Free();
}
