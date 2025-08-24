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

	/* IMGUI 렌더 */
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	m_pGameInstance->Render_End();

	return S_OK;
}

HRESULT CEditApp::Ready_Default_Setting()
{
	D3D11_BLEND_DESC Desc = {};

	Desc.AlphaToCoverageEnable = false;
	Desc.IndependentBlendEnable = false;
	Desc.RenderTarget[0].BlendEnable = true;
	Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // 소스 알파 값
	Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // 대상 알파 값의 역수
	Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // 더하기 연산
	Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE; // 소스 알파 값을 1,
	Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO; // 대상 알파 값 0로.
	Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD; // 더하기 연산
	Desc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	ID3D11BlendState* pBlendState;
	if (FAILED(m_pDevice->CreateBlendState(&Desc, &pBlendState)))
		return E_FAIL;

	m_pContext->OMSetBlendState(pBlendState, nullptr, 0xffffffff);
	Safe_Release(pBlendState);

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
