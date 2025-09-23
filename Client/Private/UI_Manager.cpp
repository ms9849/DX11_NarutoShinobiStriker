#include "UI_Manager.h"

#include "GameInstance.h"
#include "DialogUI.h"
#include "ComboKOPanel.h"
#include "SkillSlotPanel.h"
#include "AttackTypePanel.h"
#include "MissionAlertPanel.h"
#include "WinPanel.h"

CUI_Manager::CUI_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUI_Manager::Initialize()
{
	return S_OK;
}


void CUI_Manager::Set_Dialog(CDialogUI* pDialogUI)
{
	if (nullptr != m_pDialogUI)
		Safe_Release(m_pDialogUI);

	m_pDialogUI = pDialogUI;
	Safe_AddRef(m_pDialogUI);
}

void CUI_Manager::Set_SkillSlotPanel(CSkillSlotPanel* pSkillSlotPanel)
{
	if (nullptr != m_pSkillSlotPanel)
		Safe_Release(m_pSkillSlotPanel);

	m_pSkillSlotPanel = pSkillSlotPanel;
	Safe_AddRef(m_pSkillSlotPanel);
}

void CUI_Manager::Set_AttackTypePanel(CAttackTypePanel* pAttackTypePanel)
{
	if (nullptr != m_pAttackTypePanel)
		Safe_Release(m_pAttackTypePanel);

	m_pAttackTypePanel = pAttackTypePanel;
	Safe_AddRef(m_pAttackTypePanel);
}

void CUI_Manager::Set_ComboKoPanel(CComboKOPanel* pComboKOPanel)
{
	if (nullptr != m_pComboKOPanel)
		Safe_Release(m_pComboKOPanel);

	m_pComboKOPanel = pComboKOPanel;
	Safe_AddRef(m_pComboKOPanel);
}

void CUI_Manager::Set_MissionAlertPanel(CMissionAlertPanel* pMissionAlertPanel)
{
	if (nullptr != m_pMissionAlertPanel)
		Safe_Release(m_pMissionAlertPanel);

	m_pMissionAlertPanel = pMissionAlertPanel;
	Safe_AddRef(m_pMissionAlertPanel);
}

void CUI_Manager::Set_WinPanel(CWinPanel* pWinPanel)
{
	if (nullptr != m_pWinPanel)
		Safe_Release(m_pWinPanel);

	m_pWinPanel = pWinPanel;
	Safe_AddRef(m_pWinPanel);
}

void CUI_Manager::Set_Dialog_Text(const _wstring& strDialogText)
{
	m_pDialogUI->Set_Text(strDialogText);
}

void CUI_Manager::Set_Dialog_Visible(_bool bFlag)
{
	m_pDialogUI->Set_Visible(bFlag);
}

void CUI_Manager::Active_Combo()
{
	m_pComboKOPanel->Active_Combo();
}

void CUI_Manager::Active_KO()
{
	m_pComboKOPanel->Active_KO();
}

void CUI_Manager::Change_Skill(_uint iIdx, SKILL eSkill)
{
	m_pSkillSlotPanel->Change_Skill(iIdx, eSkill);
}

void CUI_Manager::Set_SkillSlot_Visible(_bool bFlag)
{
	m_pSkillSlotPanel->Set_SkillSlot_Visible(bFlag);
}

void CUI_Manager::Change_AttackType(ATTACK_TYPE eAttackType)
{
	m_pAttackTypePanel->Change_AttackType(eAttackType);
}

void CUI_Manager::Set_AttackType_Visible(_bool bFlag)
{
	m_pAttackTypePanel->Set_AttackType_Visible(bFlag);
}

void CUI_Manager::AlertPanel_Start_FadeIn(const _wstring& strMissionText)
{
	m_pMissionAlertPanel->AlertPanel_Start_FadeIn(strMissionText);
}

void CUI_Manager::WinPanel_Start_FadeIn()
{
	m_pWinPanel->WinPanel_Start_FadeIn();
}

CUI_Manager* CUI_Manager::Create()
{
	return new CUI_Manager();
}

void CUI_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDialogUI);
	Safe_Release(m_pAttackTypePanel);
	Safe_Release(m_pComboKOPanel);
	Safe_Release(m_pSkillSlotPanel);
	Safe_Release(m_pMissionAlertPanel);
	Safe_Release(m_pWinPanel);
}
