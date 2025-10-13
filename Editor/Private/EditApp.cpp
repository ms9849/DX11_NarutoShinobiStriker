#include "EditApp.h"

#include "GameInstance.h"
#include "Level_Edit.h"

CEditApp::CEditApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEditApp::Initialize()
{
	ENGINE_DESC				EngineDesc{};
	EngineDesc.hInstance = g_hInstance;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWindowMode = WINMODE::WIN;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iNumLevels = ENUM_CLASS(LEVEL::END);

	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Default_Setting()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::EDIT)))
		return E_FAIL;

	//m_pGameInstance->PlayBGM(TEXT("test2.mp3"), 0.5f);

	return S_OK;
}

void CEditApp::Update(_float fTimeDelta)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CEditApp::Render()
{
	_float4			vClearColor = _float4(0.8f, 0.8f, 0.8f, 1.f);

	m_pGameInstance->Render_Begin(&vClearColor);

	m_pGameInstance->Draw();

	/* IMGUI ·»´õ */
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	m_pGameInstance->Render_End();

	return S_OK;
}

HRESULT CEditApp::Ready_Default_Setting()
{
	return S_OK;
}

HRESULT CEditApp::Start_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Change_Level(CLevel_Edit::Create(m_pDevice, m_pContext, LEVEL::EDIT))))
		return E_FAIL;

	return S_OK;
}

CEditApp* CEditApp::Create()
{
	CEditApp* pInstance = new CEditApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CEditApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEditApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pGameInstance->Release_Engine();
	Safe_Release(m_pGameInstance);
}
