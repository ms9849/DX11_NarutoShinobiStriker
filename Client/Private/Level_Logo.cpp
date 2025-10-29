#include "Level_Logo.h"

#include "UIObject.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Level_Loading.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel { pDevice, pContext, ENUM_CLASS(eLevelID)}
	, m_pGameManager { CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;	

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	m_pGameInstance->PlayBGM(TEXT("Title_BG.wav"), 0.6f);

	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	//Pooling_Test(fTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_F3))
	{
		m_pGameInstance->Set_Visible_IMGUI(true, ENUM_CLASS(IMGUI_VISIBLE::GAMEINFO));
	}

	if (m_pGameInstance->Key_Down(DIK_F4))
	{
		m_pGameInstance->Set_Visible_IMGUI(false, ENUM_CLASS(IMGUI_VISIBLE::GAMEINFO));
	}

	if (m_pGameInstance->IsLevelChangeRequested())
	{
		m_pGameManager->Clear();

		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::OUTFITSELECT))))
			return;
	}
}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("로고 레벨"));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI(const _wstring& strLayerTag)
{
	CUIObject::UIOBJECT_DESC Desc = CUIObject::CreateDesc(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f, 0.5f, g_iWinSizeX, g_iWinSizeY, 0, 0.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_LogoPanel"),
		ENUM_CLASS(LEVEL::LOGO), strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}


CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();

	Safe_Release(m_pGameManager);
}


//void CLevel_Logo::Pooling_Test(_float fTimeDelta)
//{
//	m_fTimeAcc += fTimeDelta;
//
//	if (m_fTimeAcc >= 0.233f)
//	{
//		m_pGameInstance->Add_PoolingObject_ToLayer(
//			TEXT("Object_Dummy"),
//			ENUM_CLASS(LEVEL::LOGO),
//			nullptr, /* OBJECT_DESC 넘기는 부분. */
//			ENUM_CLASS(LEVEL::LOGO),
//			TEXT("Layer_Dummy")
//		);
//
//		m_fTimeAcc = 0.f;
//	}
//}

//HRESULT CLevel_Logo::Ready_Layer_Dummy(const _wstring& strLayerTag)
//{
//	/* 풀링 컴포넌트 사용법. */
//	for (_int i = 0; i < 100; ++i)
//	{
//		m_pGameInstance->Add_GameObject_ToPool(ENUM_CLASS(LEVEL::LOGO), static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::LOGO),
//			TEXT("Prototype_GameObject_Dummy"), nullptr)));
//	}
//
//	for (_int i = 0; i < 50; ++i)
//		m_pGameInstance->Add_PoolingObject_ToLayer(TEXT("Object_Dummy"), ENUM_CLASS(LEVEL::LOGO), nullptr, ENUM_CLASS(LEVEL::LOGO), TEXT("Layer_Dummy"));
//
//	return S_OK;
//}