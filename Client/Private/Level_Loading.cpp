#include "Level_Loading.h"

#include "Loader.h"
#include "GameInstance.h"
#include "GameManager.h"

#include "Level_Logo.h"
#include "Level_OutfitSelect.h"
#include "Level_Tutorial.h"
#include "Level_KonohaVillage.h"
#include "LoadingBarPanel.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel { pDevice, pContext, ENUM_CLASS(eLevelID)}
	, m_pGameManager { CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;
	m_pGameManager->Set_NextLevelID(m_eNextLevelID);
	/* 다음 레벨에 대한 자원을 로드하여 준비해둔다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	///* 이 레벨을 구성하기위한 객체를 만든다. */
	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	if (true == m_pLoader->isFinished())
	{
		CLevel* pNewLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case LEVEL::LOGO:
			pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		case LEVEL::OUTFITSELECT:
			pNewLevel = CLevel_OutfitSelect::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		case LEVEL::TUTORIAL:
			pNewLevel = CLevel_Tutorial::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		case LEVEL::KONOHA_VILLAGE:
			pNewLevel = CLevel_KonohaVillage::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		}

		if (FAILED(m_pGameInstance->Change_Level(pNewLevel)))
			return;		
	}
	else
		m_pLoadingBarPanel->Set_LoadingProgress(m_pLoader->Get_LoadingProgress());
}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->Output();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_UI(const _wstring& strLayerTag)
{
	CUIObject::UIOBJECT_DESC Desc = CUIObject::CreateDesc(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f, 0.5f, g_iWinSizeX, g_iWinSizeY, 0, 0.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_LoadingPanel"),
		ENUM_CLASS(LEVEL::LOADING), strLayerTag, &Desc)))
		return E_FAIL;

	Desc = CUIObject::CreateDesc(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f + 280, 0.45f, g_iWinSizeX - 300, 10, 0, 0.f);

	m_pLoadingBarPanel = static_cast<CLoadingBarPanel*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_LoadingBarPanel"), &Desc));
	Safe_AddRef(m_pLoadingBarPanel);

	m_pLoadingBarPanel->Set_MaxLoadingProgress(m_pLoader->Get_MaxLoadingProgress());
	m_pGameInstance->Add_Clone_ToLayer(m_pLoadingBarPanel, ENUM_CLASS(LEVEL::LOADING), strLayerTag);

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID, LEVEL eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoadingBarPanel);
	Safe_Release(m_pLoader);
	Safe_Release(m_pGameManager);
}
