#include "UI_Manager.h"

#include "GameInstance.h"
#include "DialogUI.h"

CUI_Manager::CUI_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUI_Manager::Initialize()
{
	return S_OK;
}

void CUI_Manager::Set_Dialog_Text(const _wstring& strDialogText)
{
	m_pDialogUI->Set_Text(strDialogText);
}

void CUI_Manager::Set_Dialog_Visible(_bool bFlag)
{
	m_pDialogUI->Set_Visible(bFlag);
}

void CUI_Manager::Add_Dialog(CDialogUI* pDialogUI)
{
	m_pDialogUI = pDialogUI;
	Safe_AddRef(m_pDialogUI);
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
}
