#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"

#include "Orthogonal.h"
#include "Pooling.h"
#include "Dummy.h"

#include "LoadingPanel.h"
#include "RotationIconUI.h"

#include "LoadingBarPanel.h"
#include "ProgressBarUI.h"

#include "Model.h"
#include "Player.h"
#include "Props.h"
#include "Gate.h"
#include "Tree.h"
#include "Font.h"

#include "Effect_CoolDown.h"

#include "GameManager.h"

/* 테스트 브랜치용 주석 */
CMainApp::CMainApp()	
	: m_pGameInstance { CGameInstance::GetInstance() }
	, m_pGameManager { CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pGameManager);
}

HRESULT CMainApp::Initialize()
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

	if (FAILED(m_pGameManager->Initialize_GameManager()))
		return E_FAIL;

	if (FAILED(Ready_Default_Setting()))
		return E_FAIL;

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::LOGO)))
		return E_FAIL;

	//m_pGameInstance->PlayBGM(TEXT("test2.mp3"), 0.5f);

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	_float4			vClearColor = _float4(0.f, 0.f, 1.f, 1.f);

	m_pGameInstance->Render_Begin(&vClearColor);

	m_pGameInstance->Draw();

	/* IMGUI 렌더 */
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	m_pGameInstance->Render_End();

	return S_OK;
}

HRESULT CMainApp::Ready_Default_Setting()
{
	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, eLevelID))))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CMainApp::Ready_Prototypes()
{
#pragma region GAMEOBJECT

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext, OBJECTID::PLAYER))))
		return E_FAIL;

	/* For.Prototype_GameObject_Dummy */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Dummy"),
		CDummy::Create(m_pDevice, m_pContext, OBJECTID::DUMMY))))
		return E_FAIL;

	/* For.Prototype_GameObject_Props */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Props"),
		CProps::Create(m_pDevice, m_pContext, OBJECTID::PROPS))))
		return E_FAIL;

	/* For.Prototype_GameObject_Gate */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Gate"),
		CGate::Create(m_pDevice, m_pContext, OBJECTID::GATE))))
		return E_FAIL;

	/* For.Prototype_GameObject_Tree */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Tree"),
		CTree::Create(m_pDevice, m_pContext, OBJECTID::TREE))))
		return E_FAIL;

#pragma endregion

#pragma region MODEL_FBX
	_matrix			PreTransformMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_Tree */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Tree"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Tree/Tree.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Gate */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Gate"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Gate/Gate.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Props */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Props"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Props/Props.fbx", PreTransformMatrix))))
		return E_FAIL;
	
	/* For.Prototype_Component_Model_WhiteJetsu */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_WhiteJetsu"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/WhiteJetsu/WhiteJetsu.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Bird */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Bird"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/Bird/Bird.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Fiona */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Upper_Player */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Upper_Player"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Player/Upper_Player.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Lower_Player */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Lower_Player"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Player/Lower_Player.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Face_Player */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Face_Player"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Player/Face_Player.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Head_Player */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Head_Player"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Player/Head_Player.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_Player */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Weapon_Player"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Player/Weapon_Player.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion

#pragma region MODEL_BINARY

	/* For.Prototype_Component_Model_TutorialMap */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_TutorialMap"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, TEXT("../Bin/Resources/Models/TutorialMap/TutorialMap.bin"), PreTransformMatrix))))
		return E_FAIL;

#pragma endregion

#pragma region TEXTURE
	/* For.Prototype_Component_Texture_SkillCoolDownEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SkillCoolDownEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/SkillSlot/SkillCoolDownEffect.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_EnemyHPBar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_EnemyHPBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EnemyHPBar/EnemyHPBar.png"), 1))))
		return E_FAIL;

#pragma endregion

#pragma region COMPONENT

	/* For.Prototype_Component_Test_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Test_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
		return E_FAIL;

	/* For.Prototype_Component_Pooling*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Pooling"),
		CPooling::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Orthogonal */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Orthogonal"),
		COrthogonal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Font */
  	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Font"),
		CFont::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Fonts/MyFont.spritefont")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region SHADER

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;
	
	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

#pragma endregion

#pragma region UI
	/* For.Prototype_Component_Texture_LoadingPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Loading/LoadingPanel.png"), 1))))
		return E_FAIL;

	/* For.Prototype_GameObject_LoadingPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_LoadingPanel"),
		CLoadingPanel::Create(m_pDevice, m_pContext, OBJECTID::LOADING_PANEL))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_RotationIconUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_RotationIconUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Loading/RotationIcon.png"), 1))))
		return E_FAIL;

	/* For.Prototype_GameObject_RotationIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_RotationIconUI"),
		CRotationIconUI::Create(m_pDevice, m_pContext, OBJECTID::ROTATIONICON_UI))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_LoadingBarPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingBarPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Loading/LoadingBarPanel.png"), 1))))
		return E_FAIL;

	/* For.Prototype_GameObject_LoadingBarPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_LoadingBarPanel"),
		CLoadingBarPanel::Create(m_pDevice, m_pContext, OBJECTID::LOADINGBAR_PANEL))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_ProgressBarUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_ProgressBarUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/ProgressBar/ProgressBar%d.png"), 3))))
		return E_FAIL;

	/* For.Prototype_GameObject_ProgressBarUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ProgressBarUI"),
		CProgressBarUI::Create(m_pDevice, m_pContext, OBJECTID::PROGRESSBAR_UI))))
		return E_FAIL;
#pragma endregion
	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pGameManager->Release_GameManager();
	Safe_Release(m_pGameManager);

	m_pGameInstance->Release_Engine();
	Safe_Release(m_pGameInstance);
}
