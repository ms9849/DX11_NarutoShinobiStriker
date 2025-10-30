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
#include "TriggerBox.h"
#include "ParticleObject.h"

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

#pragma region EFFECT

#include "EffectContainer.h"
#include "EffectObject.h"
#include "Trail.h"

#pragma endregion

#include "Effect_HitSprite.h"
#include "Collider.h"
#include "GameManager.h"
#include "SkyMesh.h"
#include "ThousandArm.h"
#include "WoodArm.h"

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

	ShowCursor(false);

	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(m_pGameManager->Initialize_GameManager()))
		return E_FAIL;
	
	if (FAILED(Ready_Default_Setting()))
		return E_FAIL;

	if (FAILED(Ready_PlayerOutfits()))
		return E_FAIL;

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if(FAILED((Ready_Effects())))
		return E_FAIL;

	if (FAILED(Ready_Particles()))
		return E_FAIL;

	if (FAILED(Bind_PreSRVS()))
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
		CTriggerBox::Create(m_pDevice, m_pContext, OBJECTID::MONSTER_SPAWNER))))
		return E_FAIL;

	/* For.Prototype_GameObject_ThousandArm*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ThousandArm"),
		CThousandArm::Create(m_pDevice, m_pContext, OBJECTID::THOUSAND_ARM))))
		return E_FAIL;

	/* For.Prototype_GameObject_WoodArm*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_WoodArm"),
		CWoodArm::Create(m_pDevice, m_pContext, OBJECTID::WOODARM))))
		return E_FAIL;
#pragma endregion

#pragma region PLAYER
	///* For.Prototype_Component_Model_Upper_Player */
	//PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Upper_Player"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Player/Upper_Player.fbx", PreTransformMatrix))))
	//	return E_FAIL;
	//m_pGameManager->Add_Outfit(SELECT_TYPE::UPPER, TEXT("테스트 상의"), TEXT("Prototype_Component_Model_Upper_Player"));

	///* For.Prototype_Component_Model_Lower_Player */
	//PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Lower_Player"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Player/Lower_Player.fbx", PreTransformMatrix))))
	//	return E_FAIL;
	//m_pGameManager->Add_Outfit(SELECT_TYPE::LOWER, TEXT("테스트 하의"), TEXT("Prototype_Component_Model_Lower_Player"));

	///* For.Prototype_Component_Model_Face_Player */
	//PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Face_Player"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Player/Face_Player.fbx", PreTransformMatrix))))
	//	return E_FAIL;
	//m_pGameManager->Add_Outfit(SELECT_TYPE::FACE, TEXT("테스트 얼굴"), TEXT("Prototype_Component_Model_Face_Player"));

	///* For.Prototype_Component_Model_Head_Player */
	//PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Head_Player"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Player/Head_Player.fbx", PreTransformMatrix))))
	//	return E_FAIL;
	//m_pGameManager->Add_Outfit(SELECT_TYPE::HEAD, TEXT("테스트 머리"), TEXT("Prototype_Component_Model_Head_Player"));
		///* For.Prototype_Component_Model_Weapon_Player */
	//PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Weapon_Player"),
	//	CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Player/Weapon_Player.fbx", PreTransformMatrix))))
	//	return E_FAIL;

#pragma endregion


#pragma region MODEL_BINARY
	_matrix			PreTransformMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_SkyBox */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_SkyBox"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, TEXT("../Bin/Resources/Models/SkyBox/SkyBox.bin"), PreTransformMatrix))))
		return E_FAIL;

	/* 목둔 박수 */
	/* For.Prototype_Component_Model_WoodHand_L */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(270.0f)) * XMMatrixTranslation(-2.65f, 0.f, 0.f);
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_WoodHand_L"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../Bin/Resources/Models/WoodHand/WoodHand_L.bin"), PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WoodHand_R */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixTranslation(2.65f, 0.f, 0.f);
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_WoodHand_R"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../Bin/Resources/Models/WoodHand/WoodHand_R.bin"), PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_NPC_Kakashi*/
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_NPC_Kakashi"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../Bin/Resources/Models/NPC/NPC_Kakashi.Bin"), PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Kunai*/
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Kunai"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, TEXT("../Bin/Resources/Models/Kunai/Kunai.bin"), PreTransformMatrix))))
		return E_FAIL;


	/* For.Prototype_Component_Model_Kunai_Diff*/
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Kunai_Diff"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, TEXT("../Bin/Resources/Models/Kunai/Kunai2.bin"), PreTransformMatrix))))
		return E_FAIL;

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

	/* 복서 관련 컴포넌트 */
/* For.Prototype_Component_Model_Weapon_Boxer_L */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Weapon_Boxer_L"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, TEXT("../Bin/Resources/Models/Monster/Boxer/L_Boxer_Glove.bin"), PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_Boxer_R */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationX(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Weapon_Boxer_R"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, TEXT("../Bin/Resources/Models/Monster/Boxer/R_Boxer_Glove.bin"), PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Head_Boxer */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Head_Boxer"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../Bin/Resources/Models/Monster/Boxer/Head_Boxer.bin"), PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::HEAD, TEXT("개굴맨 모자"), TEXT("Prototype_Component_Model_Head_Boxer"));


	/* For.Prototype_Component_Model_Face_Boxer */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Face_Boxer"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../Bin/Resources/Models/Monster/Boxer/Face_Boxer.bin"), PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Upper_Boxer */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Upper_Boxer"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../Bin/Resources/Models/Monster/Boxer/Upper_Boxer.bin"), PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::ONE_CLOTH, TEXT("개굴맨"), TEXT("Prototype_Component_Model_Upper_Boxer"));

	/* 파자마 */
	/* For.Prototype_Component_Model_Head_Pajama */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Head_Pajama"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../Bin/Resources/Models/Monster/Pajama/Head_Pajama.bin"), PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::HEAD, TEXT("파자마 모자"), TEXT("Prototype_Component_Model_Head_Pajama"));


	/* For.Prototype_Component_Model_Face_Pajama */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Face_Pajama"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../Bin/Resources/Models/Monster/Pajama/Face_Pajama.bin"), PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Upper_Pajama */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Upper_Pajama"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../Bin/Resources/Models/Monster/Pajama/Upper_Pajama.bin"), PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::ONE_CLOTH, TEXT("파자마"), TEXT("Prototype_Component_Model_Upper_Pajama"));

	/* 보스 */
	/* For.Prototype_Component_Model_Head_Boss */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Head_Boss"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../Bin/Resources/Models/Monster/Boss/Head_Boss.bin"), PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Face_Boss*/
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Face_Boss"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../Bin/Resources/Models/Monster/Boss/Face_Boss.bin"), PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Upper_Boss */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Upper_Boss"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../Bin/Resources/Models/Monster/Boss/Upper_Boss.bin"), PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_1000Arm_Arm */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_1000Arm"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, TEXT("../../Client/Bin/Resources/Models/1000hand/1000hand.bin"), PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_1000Arm_Arm */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationX(XMConvertToRadians(90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_1000Arm_Arm"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, TEXT("../../Client/Bin/Resources/Models/1000hand/1000hand_arm.bin"), PreTransformMatrix))))
		return E_FAIL;

#pragma endregion

#pragma region TEXTURE
	/* For.Prototype_Component_Texture_Sky_Day */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Sky_Day"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/BackGround/SkyBox%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_SwordTrail */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SwordTrail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/SwordTrail.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_FootTrail */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_FootTrail_Blue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/FootTrail_Blue.png"), 1))))
		return E_FAIL;


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

	/* For.Prototype_Component_Texture_HitEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_HitEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/HitEffect/HitEffect%d.png"), 3))))
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

	/* For.Prototype_GameObject_SwordTrail */
	CTrail::PROTOTYPE_TRAIL_DESC Desc;
	Desc.iNumVertices = 44;
	Desc.eType = CTrail::TRAIL_TYPE::SWORD;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SwordTrail"),
		CTrail::Create(m_pDevice, m_pContext, OBJECTID::TRAIL, &Desc))))
		return E_FAIL;

	Desc.iNumVertices = 880;
	Desc.eType = CTrail::TRAIL_TYPE::FOOT;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_FootTrail"),
		CTrail::Create(m_pDevice, m_pContext, OBJECTID::TRAIL, &Desc))))
		return E_FAIL;

	Desc.iNumVertices = 220;
	Desc.eType = CTrail::TRAIL_TYPE::FOOT;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_KunaiTrail"),
		CTrail::Create(m_pDevice, m_pContext, OBJECTID::TRAIL, &Desc))))
		return E_FAIL;
#pragma endregion

#pragma region SHADER
	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

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

	return S_OK;
}

HRESULT CMainApp::Ready_Effects()
{
#pragma region HITEFFECT
	/* For.Prototype_GameObject_HitEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_HitEffect"),
		CEffect_HitSprite::Create(m_pDevice, m_pContext, OBJECTID::EFFECT))))
		return E_FAIL;
#pragma endregion
#pragma region SHADEREFFECT
	/* For.Prototype_Component_Shader_VtxEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxEffect.hlsl"), VTXEFFMESH::Elements, VTXEFFMESH::iNumElements))))
		return E_FAIL;
#pragma endregion

#pragma region DIFFUSE
	/* For.Prototype_Component_Texture_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Effect_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Diffuse/Diffuse%d.png"), 36))))
		return E_FAIL;
#pragma endregion

#pragma region MASK
	/* For.Prototype_Component_Texture_Effect_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Effect_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Mask/Mask%d.png"), 103))))
		return E_FAIL;
#pragma endregion

#pragma region NOISE
	/* For.Prototype_Component_Texture_Effect_Noise */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Effect_Noise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Noise/Noise%d.png"), 37))))
		return E_FAIL;
#pragma endregion

#pragma region BINARY
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(L"../Bin/Resources/Models/Effect/*.bin", &fd);
	if (hFind == INVALID_HANDLE_VALUE) return E_FAIL;

	do {
		/* For.Prototype_Component_EffectModel_Test */
		_fmatrix PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
		_wstring strFileName = wstring(fd.cFileName);

		strFileName.erase(strFileName.size() - 4);

		_wstring strComponentTag = TEXT("Prototype_Component_EffectModel_") + strFileName;
		_wstring strFilePath = TEXT("../Bin/Resources/Models/Effect/") + strFileName + TEXT(".bin");

		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), strComponentTag,
			CEffectModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, strFilePath.c_str(), PreTransformMatrix))))
			return E_FAIL;
	} while (FindNextFile(hFind, &fd));
#pragma endregion

	/* For.Prototype_GameObject_EffectContainer */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EffectContainer"),
		CEffectContainer::Create(m_pDevice, m_pContext, OBJECTID::EFFECTCONTAINER))))
		return E_FAIL;

	/* For.Prototype_GameObject_EffectObject */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EffectObject"),
		CEffectObject::Create(m_pDevice, m_pContext, OBJECTID::EFFECTOBJECT))))
		return E_FAIL;

	/* For.Prototype_GameObject_SkyMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkyMesh"),
		CSkyMesh::Create(m_pDevice, m_pContext, OBJECTID::SKYBOX))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Particles()
{
	/* For.Prototype_Component_Texture_Dissolve */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Dissolve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Dissolve/Dissolve%d.png"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Particle */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Particle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Particle/Particle%d.png"), 3))))
		return E_FAIL;

	/* 파티클 원형 생성 */
	/* For.Prototype_GameObject_ParticleObject */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"),
		CParticleObject::Create(m_pDevice, m_pContext, OBJECTID::PARTICLE))))
		return E_FAIL;

	/* 바인딩만 수행하고 사라지게 해. */
	CTexture* pTextureCom = static_cast<CTexture*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Dissolve")));
	CShader* pShaderCom = static_cast<CShader*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPointParticle")));

	pTextureCom->Bind_ShaderResource(pShaderCom, "g_DissolveTexture", 0);
	Safe_Release(pTextureCom);
	Safe_Release(pShaderCom);

	return S_OK;
}

HRESULT CMainApp::Ready_PlayerOutfits()
{
	_matrix PreTransformMatrix;
#pragma region PLAYER_FACE
	///* 얼굴 3종*/
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Face_Player_Bandage"),
	    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Face_Player.bin"), PreTransformMatrix))))
	    return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::FACE, TEXT("붕대"), TEXT("Prototype_Component_Model_Face_Player_Bandage"));

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Face_Player_Default"),
	    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Face_Player_Default.bin"), PreTransformMatrix))))
	    return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::FACE, TEXT("기본 얼굴"), TEXT("Prototype_Component_Model_Face_Player_Default"));


	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Face_Player_Mask"),
	    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Face_Player_Mask.bin"), PreTransformMatrix))))
	    return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::FACE, TEXT("마스크"), TEXT("Prototype_Component_Model_Face_Player_Mask"));

#pragma endregion


#pragma region PLAYER_HEAD
	/* 머리 */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Head_Player_SnowMan"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Head_Player_SnowMan.bin"), PreTransformMatrix))))
		return E_FAIL;

	m_pGameManager->Add_Outfit(SELECT_TYPE::HEAD, TEXT("스-노우맨"), TEXT("Prototype_Component_Model_Head_Player_SnowMan"));
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Head_Player"),
	    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Head_Player.bin"), PreTransformMatrix))))
	    return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::HEAD, TEXT("포니 테일"), TEXT("Prototype_Component_Model_Head_Player"));

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Head_Player_Allback"),
	    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Head_Player_Allback.bin"), PreTransformMatrix))))
	    return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::HEAD, TEXT("올백 머리"), TEXT("Prototype_Component_Model_Head_Player_Allback"));

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Head_Player_Hat"),
	    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Head_Player_Hat.bin"), PreTransformMatrix))))
	    return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::HEAD, TEXT("인형술사 모자"), TEXT("Prototype_Component_Model_Head_Player_Hat"));

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Head_Player_Sanbal"),
	    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Head_Player_Sanbal.bin"), PreTransformMatrix))))
	    return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::HEAD, TEXT("산발 머리"), TEXT("Prototype_Component_Model_Head_Player_Sanbal"));
#pragma endregion


#pragma region PLAYER_LOWER

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Lower_Sasuke"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Lower_Player_Sasuke.bin"), PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::LOWER, TEXT("사스케"), TEXT("Prototype_Component_Model_Lower_Sasuke"));

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Lower_Player"),
	    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Lower_Player.bin"), PreTransformMatrix))))
	    return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::LOWER, TEXT("나뭇잎 마을"), TEXT("Prototype_Component_Model_Lower_Player"));

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Lower_Ambu"),
	    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Lower_Player_Ambu.bin"), PreTransformMatrix))))
	    return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::LOWER, TEXT("암부 하의"), TEXT("Prototype_Component_Model_Lower_Ambu"));

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Lower_Durumari"),
	    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Lower_Player_Durumari.bin"), PreTransformMatrix))))
	    return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::LOWER, TEXT("두루마리"), TEXT("Prototype_Component_Model_Lower_Durumari"));

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Lower_Orochimaru"),
	    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Lower_Player_Orochimaru.bin"), PreTransformMatrix))))
	    return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::LOWER, TEXT("오로치마루"), TEXT("Prototype_Component_Model_Lower_Orochimaru"));

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Lower_Shoichi"),
	    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Lower_Player_Shoichi.bin"), PreTransformMatrix))))
	    return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::LOWER, TEXT("평범한 회사원"), TEXT("Prototype_Component_Model_Lower_Shoichi"));

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Lower_Stone"),
	    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Lower_Player_Stone.bin"), PreTransformMatrix))))
	    return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::LOWER, TEXT("바위 마을"), TEXT("Prototype_Component_Model_Lower_Stone"));

#pragma endregion


#pragma region PLAYER_ONECLOTH

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_OneCloth_Akastuki"),
	    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/OneCloth_Player_Akastuki.bin"), PreTransformMatrix))))
	    return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::ONE_CLOTH, TEXT("아카츠키"), TEXT("Prototype_Component_Model_OneCloth_Akastuki"));

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_OneCloth_Player_Beach"),
	    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/OneCloth_Player_Beach.bin"), PreTransformMatrix))))
	    return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::ONE_CLOTH, TEXT("수영복"), TEXT("Prototype_Component_Model_OneCloth_Player_Beach"));

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_OneCloth_Player_SnowMan"),
	    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/OneCloth_Player_SnowMan.bin"), PreTransformMatrix))))
	    return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::ONE_CLOTH, TEXT("스-노우맨"), TEXT("Prototype_Component_Model_OneCloth_Player_SnowMan"));


	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_OneCloth_Player_Training"),
	    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/OneCloth_Player_Training.bin"), PreTransformMatrix))))
	    return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::ONE_CLOTH, TEXT("트레이닝 복장"), TEXT("Prototype_Component_Model_OneCloth_Player_Training"));

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_OneCloth_Player_Tuxedo"),
	    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/OneCloth_Player_Tuxedo.bin"), PreTransformMatrix))))
	    return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::ONE_CLOTH, TEXT("턱시도"), TEXT("Prototype_Component_Model_OneCloth_Player_Tuxedo"));


#pragma endregion


#pragma region PLAYER_UPPER
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Upper_Player_Yuljung"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Upper_Player_Yuljung.bin"), PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::UPPER, TEXT("열정 티셔츠"), TEXT("Prototype_Component_Model_Upper_Player_Yuljung"));

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Upper_Player"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Upper_Player.bin"), PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::UPPER, TEXT("나뭇잎 마을"), TEXT("Prototype_Component_Model_Upper_Player"));

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Upper_Player_Ambu"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Upper_Player_Ambu.bin"), PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::UPPER, TEXT("암부 상의"), TEXT("Prototype_Component_Model_Upper_Player_Ambu"));

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Upper_Player_Konoha"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Upper_Player_Konoha.bin"), PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::UPPER, TEXT("마이트 가이"), TEXT("Prototype_Component_Model_Upper_Player_Konoha"));

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Upper_Player_Racer"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Upper_Player_Racer.bin"), PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::UPPER, TEXT("진정한 바이커"), TEXT("Prototype_Component_Model_Upper_Player_Racer"));

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Upper_Player_Shoichi"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Upper_Player_Shoichi.bin"), PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::UPPER, TEXT("평범한 회사원"), TEXT("Prototype_Component_Model_Upper_Player_Shoichi"));

	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Upper_Player_Stone"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, TEXT("../../Client/Bin/Resources/Models/Player/Upper_Player_Stone.bin"), PreTransformMatrix))))
		return E_FAIL;
	m_pGameManager->Add_Outfit(SELECT_TYPE::UPPER, TEXT("바위 마을"), TEXT("Prototype_Component_Model_Upper_Player_Stone"));

#pragma endregion

#pragma region PLAYER_WEAPON
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Weapon_Player"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, TEXT("../../Client/Bin/Resources/Models/Player/Weapon_Player.bin"), PreTransformMatrix))))
		return E_FAIL;
#pragma endregion 

	return S_OK;
}

HRESULT CMainApp::Bind_PreSRVS()
{
	/* For.Prototype_Component_Texture_Splatting */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Splatting"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Splatting/Splatting%d.png"), 3))))
		return E_FAIL;

	/* 바인딩만 수행하고 사라지게 해. */
	CTexture* pTextureCom = static_cast<CTexture*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Splatting")));
	CShader* pShaderCom = static_cast<CShader*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh")));

	pTextureCom->Bind_ShaderResource(pShaderCom, "g_SplattingMaskTexture", 0);
	pTextureCom->Bind_ShaderResource(pShaderCom, "g_SplattingGrassTexture", 1);
	pTextureCom->Bind_ShaderResource(pShaderCom, "g_SplattingSoilTexture", 2);

	Safe_Release(pTextureCom);
	Safe_Release(pShaderCom);

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
