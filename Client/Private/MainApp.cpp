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
#include "MonsterSpawner.h"

#pragma region Skill

#include "Parts_WoodHand.h"
#include "Rasengan.h"
#include "RasenShuriken.h"
#include "Kamui.h"
#include "FireBall.h"
#include "BigShark.h"
#include "Chidori.h"
#include "Kunai.h"

#include "Rope.h"

#pragma endregion

#include "Effect_CoolDown.h"
#include "Snow.h"
#include "Explosion.h"
#include "Collider.h"
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

	/* For.Prototype_GameObject_MonsterSpawner*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MonsterSpawner"),
		CMonsterSpawner::Create(m_pDevice, m_pContext, OBJECTID::MONSTER_SPAWNER))))
		return E_FAIL;

#pragma endregion

#pragma region MODEL_FBX
	_matrix			PreTransformMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_SkyBox */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_SkyBox"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/SkyBox/SkyBox.fbx", PreTransformMatrix))))
		return E_FAIL;

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

	/* For.Prototype_Component_Model_Fiona */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_Player */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Weapon_Player"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Player/Weapon_Player.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* 복서 관련 컴포넌트 */
	/* For.Prototype_Component_Model_Weapon_Boxer_L */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Weapon_Boxer_L"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Monster/Boxer/L_Boxer_Glove.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_Boxer_R */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationX(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Weapon_Boxer_R"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Monster/Boxer/R_Boxer_Glove.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Head_Boxer */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Head_Boxer"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/Boxer/Head_Boxer.fbx", PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::HEAD, TEXT("개굴맨 모자"), TEXT("Prototype_Component_Model_Head_Boxer"));


	/* For.Prototype_Component_Model_Face_Boxer */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Face_Boxer"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/Boxer/Face_Boxer.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Upper_Boxer */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Upper_Boxer"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/Boxer/Upper_Boxer.fbx", PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::ONE_CLOTH, TEXT("개굴맨"), TEXT("Prototype_Component_Model_Upper_Boxer"));

	/* 파자마 */
	/* For.Prototype_Component_Model_Head_Pajama */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Head_Pajama"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/Pajama/Head_Pajama.fbx", PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::HEAD, TEXT("파자마 모자"), TEXT("Prototype_Component_Model_Head_Pajama"));


	/* For.Prototype_Component_Model_Face_Pajama */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Face_Pajama"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/Pajama/Face_Pajama.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Upper_Pajama */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Upper_Pajama"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/Pajama/Upper_Pajama.fbx", PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::ONE_CLOTH, TEXT("파자마"), TEXT("Prototype_Component_Model_Upper_Pajama"));

	/* 보스 */
	/* For.Prototype_Component_Model_Head_Boss */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Head_Boss"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/Boss/Head_Boss.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Face_Boss*/
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Face_Boss"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/Boss/Face_Boss.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Upper_Boss */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Upper_Boss"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/Boss/Upper_Boss.fbx", PreTransformMatrix))))
		return E_FAIL;


	/* 목둔 박수 */
	/* For.Prototype_Component_Model_WoodHand_L */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(270.0f)) * XMMatrixTranslation(-2.65f, 0.f, 0.f);
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_WoodHand_L"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/WoodHand/WoodHand_L.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WoodHand_R */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixTranslation(2.65f, 0.f, 0.f);
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_WoodHand_R"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/WoodHand/WoodHand_R.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Kakashi*/
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_NPC_Kakashi"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/NPC/NPC_Kakashi.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion

#pragma region MODEL_BINARY

	/* For.Prototype_Component_Model_TutorialMap */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_TutorialMap"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, TEXT("../Bin/Resources/Models/TutorialMap/TutorialMap.bin"), PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WhiteJetsu */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_WhiteJetsu"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../Bin/Resources/Models/Monster/WhiteJetsu/WhiteJetsu.bin"), PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Bird */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Bird"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../Bin/Resources/Models/Monster/Bird/Bird.bin"), PreTransformMatrix))))
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

	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_DialogUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_DialogUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialog/Dialog.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CutSencePanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_CutSceneUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/CutScene/CutScene.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Icon/Icon%d.png"), 3))))
		return E_FAIL;

#pragma endregion

#pragma region COMPONENT
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

	/* For.Prototype_Component_Font_Large */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Font_Large"),
		CFont::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Fonts/MyFont_Large.spritefont")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_Component_VIBuffer_Particle_Snow */
	CVIBuffer_Rect_Instance::RECT_INSTANCE_DESC		SnowDesc{};

	SnowDesc.iNumInstance = 3000;
	SnowDesc.vCenter = _float3(64.0f, 20.f, 64.0f);
	SnowDesc.vRange = _float3(128.f, 2.f, 128.f);
	SnowDesc.vSize = _float2(0.2f, 0.6f);
	SnowDesc.vLifeTime = _float2(3.f, 7.f);
	SnowDesc.vSpeed = _float2(2.f, 5.f);
	SnowDesc.isLoop = true;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Particle_Snow"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, &SnowDesc))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Particle_Explosion */
	CVIBuffer_Point_Instance::POINT_INSTANCE_DESC		ExplosionDesc{};

	ExplosionDesc.iNumInstance = 300;
	ExplosionDesc.vCenter = _float3(0.0f, 0.f, 0.0f);
	ExplosionDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ExplosionDesc.vRange = _float3(2.f, 2.f, 2.f);
	ExplosionDesc.vSize = _float2(0.2f, 0.6f);
	ExplosionDesc.vLifeTime = _float2(3.f, 7.f);
	ExplosionDesc.vSpeed = _float2(2.f, 5.f);
	ExplosionDesc.isLoop = true;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Particle_Explosion"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &ExplosionDesc))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */ 
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::SPHERE))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, COLLIDER::AABB))))
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

	/* For.Prototype_Component_Shader_VtxMesh_NonLight */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh_NonLight"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh_NonLight.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectParticle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxRectParticle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectParticle.hlsl"), VTX_POSTEX_INSTANCE_PARTICLE::Elements, VTX_POSTEX_INSTANCE_PARTICLE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointParticle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPointParticle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointParticle.hlsl"), VTX_POS_INSTANCE_PARTICLE::Elements, VTX_POS_INSTANCE_PARTICLE::iNumElements))))
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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/ProgressBar/ProgressBar%d.png"), 5))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_BossHPPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BossHPPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/BossHPBar/BossHPBar.png"), 1))))
		return E_FAIL;

	/* For.Prototype_GameObject_ProgressBarUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ProgressBarUI"),
		CProgressBarUI::Create(m_pDevice, m_pContext, OBJECTID::PROGRESSBAR_UI))))
		return E_FAIL;
#pragma endregion

#pragma region SKILLS
	
	/* For.Prototype_GameObject_Part_WoodHand */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Part_WoodHand"),
		CParts_WoodHand::Create(m_pDevice, m_pContext, OBJECTID::WOODHAND))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Rasengan */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Rasengan"),
		CRasengan::Create(m_pDevice, m_pContext, OBJECTID::RASENGAN))))
		return E_FAIL;

	/* For.Prototype_GameObject_RasenganShuriken */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_RasenShuriken"),
		CRasenShuriken::Create(m_pDevice, m_pContext, OBJECTID::RASENSHURIKEN))))
		return E_FAIL;

	/* For.Prototype_GameObject_Kamui */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Kamui"),
		CKamui::Create(m_pDevice, m_pContext, OBJECTID::KAMUI))))
		return E_FAIL;

	/* For.Prototype_GameObject_FireBall */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_FireBall"),
		CFireBall::Create(m_pDevice, m_pContext, OBJECTID::FIREBALL))))
		return E_FAIL;

	/* For.Prototype_GameObject_BigShark */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_BigShark"),
		CBigShark::Create(m_pDevice, m_pContext, OBJECTID::BIGSHARK))))
		return E_FAIL;

	/* For.Prototype_GameObject_Chidori */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Chidori"),
		CChidori::Create(m_pDevice, m_pContext, OBJECTID::CHIDORI))))
		return E_FAIL;

	/* For.Prototype_GameObject_Kunai */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Kunai"),
		CKunai::Create(m_pDevice, m_pContext, OBJECTID::KUNAI))))
		return E_FAIL;

	/* For.Prototype_GameObject_Rope */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Rope"),
		CRope::Create(m_pDevice, m_pContext, OBJECTID::ROPE))))
		return E_FAIL;

#pragma endregion
	
#pragma region PARTICLE 
	/* For.Prototype_GameObject_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Snow"),
		CSnow::Create(m_pDevice, m_pContext, OBJECTID::SNOW))))
		return E_FAIL;

	/* For.Prototype_GameObject_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Explosion"),
		CExplosion::Create(m_pDevice, m_pContext, OBJECTID::EXPLOSION))))
		return E_FAIL;
#pragma endregion

#pragma region PLAYER
	/* For.Prototype_Component_Model_Upper_Player */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Upper_Player"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Player/Upper_Player.fbx", PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::UPPER, TEXT("테스트 상의"), TEXT("Prototype_Component_Model_Upper_Player"));

	/* For.Prototype_Component_Model_Lower_Player */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Lower_Player"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Player/Lower_Player.fbx", PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::LOWER, TEXT("테스트 하의"), TEXT("Prototype_Component_Model_Lower_Player"));

	/* For.Prototype_Component_Model_Face_Player */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Face_Player"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Player/Face_Player.fbx", PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::FACE, TEXT("테스트 얼굴"), TEXT("Prototype_Component_Model_Face_Player"));

	/* For.Prototype_Component_Model_Head_Player */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Head_Player"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Player/Head_Player.fbx", PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::HEAD, TEXT("테스트 머리"), TEXT("Prototype_Component_Model_Head_Player"));


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
