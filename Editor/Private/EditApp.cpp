#include "EditApp.h"

#include "GameInstance.h"
#include "Level_Edit.h"

#include "Terrain.h"
#include "TestCamera.h"
#include "Pooling.h"

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

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::EDIT)))
		return E_FAIL;

	//m_pGameInstance->PlayBGM(TEXT("test2.mp3"), 0.5f);

	return S_OK;
}

void CEditApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CEditApp::Render()
{
	_float4			vClearColor = _float4(0.8f, 0.8f, 0.8f, 1.f);

	m_pGameInstance->Render_Begin(&vClearColor);

	m_pGameInstance->Draw();

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

HRESULT CEditApp::Ready_Prototypes()
{

#pragma region TEXTURE
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Terrain/Tile0.jpg"), 1))))
		return E_FAIL;

#pragma endregion

#pragma region COMPONENT 
	_matrix			PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	/* For.Prototype_Component_Model_Fiona */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Client/Bin/Resources/Models/Fiona/Fiona.fbx"))))
		return E_FAIL;

	/* For.Prototype_Component_Pooling*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Pooling"),
		CPooling::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region BUFFER 
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Terrain/Height1.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region SHADER
	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;
#pragma endregion

#pragma region OBJECT
	/* For.Prototype_GameObject_TestCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_TestCamera"),
		CTestCamera::Create(m_pDevice, m_pContext, Client::OBJECTID::TEST_CAMERA))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext, Client::OBJECTID::TERRAIN))))
		return E_FAIL;
#pragma endregion

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
