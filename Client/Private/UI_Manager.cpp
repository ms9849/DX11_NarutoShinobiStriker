#include "UI_Manager.h"

#include "GameInstance.h"
#include "DialogUI.h"
#include "ComboKOPanel.h"
#include "SkillSlotPanel.h"
#include "AttackTypePanel.h"

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
	m_pDialogUI = pDialogUI;
	Safe_AddRef(m_pDialogUI);
}

void CUI_Manager::Set_SkillSlotPanel(CSkillSlotPanel* pSkillSlotPanel)
{
	m_pSkillSlotPanel = pSkillSlotPanel;
	Safe_AddRef(m_pSkillSlotPanel);
}

void CUI_Manager::Set_AttackTypePanel(CAttackTypePanel* pAttackTypePanel)
{
	m_pAttackTypePanel = pAttackTypePanel;
	Safe_AddRef(m_pAttackTypePanel);
}

void CUI_Manager::Set_ComboKoPanel(CComboKOPanel* pComboKOPanel)
{
	m_pComboKOPanel = pComboKOPanel;
	Safe_AddRef(m_pComboKOPanel);
}

void CUI_Manager::Set_Dialog_Text(const _wstring& strDialogText)
{
	m_pDialogUI->Set_Text(strDialogText);
}

void CUI_Manager::Set_Dialog_Visible(_bool bFlag)
{
	m_pDialogUI->Set_Visible(bFlag);
}

void CUI_Manager::Update_Combo(_uint iComboCount)
{
	m_pComboKOPanel->Update_Combo(iComboCount);
}

void CUI_Manager::PopUp_KO()
{
	m_pComboKOPanel->PopUp_KO();
}

void CUI_Manager::Change_Skill(_uint iIdx, SKILL eSkill)
{
	m_pSkillSlotPanel->Change_Skill(iIdx, eSkill);
}

void CUI_Manager::Change_AttackType(ATTACK_TYPE eAttackType)
{
	m_pAttackTypePanel->Change_AttackType(eAttackType);
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
}
