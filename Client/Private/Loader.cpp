#include "Loader.h"

#include "Dummy.h"
#include "Terrain.h"
#include "TestCamera.h"

#include "TestPanel.h"
#include "TestButton.h"

#include "LogoPanel.h"
#include "TitleUI.h"
#include "PressAnyButtonUI.h"

#include "OutfitSelectCamera.h"
#include "Mannequin.h"
#include "OutfitSelectPanel.h"
#include "CreateCharacterUI.h"
#include "ModelPanel.h"
#include "ModelSelectButtonUI.h"
#include "ModelDecideButtonUI.h"

#include "Effect_CoolDown.h"

#include "SkillSlotPanel.h"
#include "SkillSlotUI.h"
#include "DialogUI.h"
#include "TimerPanel.h"
#include "DecimalUI.h"
#include "AttackTypePanel.h"
#include "MissionAlertPanel.h"
#include "ComboKOPanel.h"
#include "KOUI.h"
#include "WinPanel.h"

#include "GameInstance.h"
#include "Model.h"

#include "Col_KonohaVillage.h"
#include "KonohaVillage.h"
#include "Col_TutorialMap.h"
#include "TutorialMap.h"

#include "MainCamera.h"
#include "ActionCamera.h"
#include "SkillActionCamera.h"
#include "NPCTalkCamera.h"

#include "Lower_Character.h"
#include "Upper_Character.h"
#include "Face_Character.h"
#include "Head_Character.h"
#include "Weapon_Character.h"

#pragma region MONSTER

#include "WhiteJetsu.h"
#include "Bird.h"
#include "Boxer.h"
#include "Pajama.h"

#include "WoodHand.h"

#pragma endregion

#include "NPC_KaKashi.h"

#include "SkyBox.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY LoadingMain(void* pArg)
{
	CLoader*		pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return 1;

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{	
	m_eNextLevelID = eNextLevelID;
	m_fLoadingProgress = 0.f;
	/* 세마포어, 뮤텍스, 크리티컬섹션 */

	/* 임계영역(힙, 데이터, 코드)에 접근하기위한 키를 생성한다. */
	InitializeCriticalSection(&m_CriticalSection);

	/* 실제 로딩을 수행하기위한 스레드를 생성한다. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{
	CoInitializeEx(nullptr, 0);

	EnterCriticalSection(&m_CriticalSection);

	HRESULT		hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL::LOGO:
		hr = Loading_For_Logo();
		break;
	case LEVEL::OUTFITSELECT:
		hr = Loading_For_OutfitSelect();
		break;
	case LEVEL::TUTORIAL:
		hr = Loading_For_Tutorial();
		break;
	case LEVEL::KONOHA_VILLAGE:
		hr = Loading_For_KonohaVillage();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

void CLoader::Output()
{
	SetWindowText(g_hWnd, m_strMessage.c_str());
}

HRESULT CLoader::Loading_For_Logo()
{
	m_fLoadingProgress += 0.4f;
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	/* For.Prototype_Component_Texture_LogoPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_LogoPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Logo/LogoPanel.jpg"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_TitleUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_TitleUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Logo/Title.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_PressAnyButtonUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_PressAnyButtonUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Logo/PressAnyButton%d.png"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_TimerPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_TimerPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Timer/Timer%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_DecimalUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_DecimalUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Decimal/Decimal%d.png"), 20))))
		return E_FAIL;

	m_fLoadingProgress += 0.2f;

	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");
	m_fLoadingProgress += 0.2f;

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");
	m_fLoadingProgress += 0.2f;

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");
	/* For.Prototype_GameObject_Dummy */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_Dummy"),
		CDummy::Create(m_pDevice, m_pContext, OBJECTID::DUMMY))))
		return E_FAIL;

	/* For.Prototype_GameObject_TestButton */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_TestButton"),
		CTestButton::Create(m_pDevice, m_pContext, OBJECTID::TESTBUTTON))))
		return E_FAIL;

	/* For.Prototype_GameObject_TestPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_TestPanel"),
		CTestPanel::Create(m_pDevice, m_pContext, OBJECTID::TESTPANEL))))
		return E_FAIL;

	/* For.Prototype_GameObject_LogoPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_LogoPanel"),
		CLogoPanel::Create(m_pDevice, m_pContext, OBJECTID::LOGO_PANEL))))
		return E_FAIL;

	/* For.Prototype_GameObject_TitleUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_TitleUI"),
		CTitleUI::Create(m_pDevice, m_pContext, OBJECTID::TITLE_UI))))
		return E_FAIL;

	/* For.Prototype_GameObject_PressAnyButtonUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_PressAnyButtonUI"),
		CPressAnyButtonUI::Create(m_pDevice, m_pContext, OBJECTID::PRESSANYBUTTON_UI))))
		return E_FAIL;

	/* For.Prototype_GameObject_DecimalUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_DecimalUI"),
		CDecimalUI::Create(m_pDevice, m_pContext, OBJECTID::DECIMAL_UI))))
		return E_FAIL;

	/* For.Prototype_GameObject_TimerPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_TimerPanel"),
		CTimerPanel::Create(m_pDevice, m_pContext, OBJECTID::TIMER_UI))))
		return E_FAIL;

	m_fLoadingProgress += 0.2f;
	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_OutfitSelect()
{
	m_fLoadingProgress += 0.4f;
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	/* For.Prototype_Component_Texture_OutfirSelectPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_Component_Texture_OutfirSelectPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/OutfitSelect/OutfitSelectPanel.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_CreateCharacterUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_Component_Texture_CreateCharacterUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/OutfitSelect/CreateCharacterUI.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_ModelPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_Component_Texture_ModelPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/ModelPanel/ModelPanel.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_ModelSelectButtonUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_Component_Texture_ModelSelectButtonUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/ModelSelectButton/ModelSelectButton%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_ModelDecideButtonUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_Component_Texture_ModelDecideButtonUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/ModelDecideButton/ModelDecideButton%d.png"), 2))))
		return E_FAIL;

	m_fLoadingProgress += 0.3f;
	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");

	/*
	테스트 용으로 집어넣은 단일 모델. 
	추후 모델 분리 배우면 모델들 다 따로 
	집어넣고 세팅해줘야 한다.
	*/

	_matrix			PreTransformMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_Mannequin */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_Component_Model_Mannequin"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Outfits/Mannequin.fbx"))))
		return E_FAIL;

	m_fLoadingProgress += 0.3f;
	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");

	/* For.Prototype_GameObject_TestCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_GameObject_TestCamera"),
		CTestCamera::Create(m_pDevice, m_pContext, OBJECTID::TEST_CAMERA))))
		return E_FAIL;

	/* For.Prototype_GameObject_OutfitSelectCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_GameObject_OutfitSelectCamera"),
		COutfitSelectCamera::Create(m_pDevice, m_pContext, OBJECTID::OUTFITSELECT_CAMERA))))
		return E_FAIL;

	/* For.Prototype_GameObject_OutfitSelectPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_GameObject_OutfitSelectPanel"),
		COutfitSelectPanel::Create(m_pDevice, m_pContext, OBJECTID::OUTFITSELECT_PANEL))))
		return E_FAIL;

	/* For.Prototype_GameObject_CreateCharacterUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_GameObject_CreateCharacterUI"),
		CCreateCharacterUI::Create(m_pDevice, m_pContext, OBJECTID::CREATECHARACTER_UI))))
		return E_FAIL;

	/* For.Prototype_GameObject_ModelPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_GameObject_ModelPanel"),
		CModelPanel::Create(m_pDevice, m_pContext, OBJECTID::MODEL_PANEL))))
		return E_FAIL;

	/* For.Prototype_GameObject_ModelSelectButtonUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_GameObject_ModelSelectButtonUI"),
		CModelSelectButtonUI::Create(m_pDevice, m_pContext, OBJECTID::MODELSELECTBUTTON_UI))))
		return E_FAIL;

	/* For.Prototype_GameObject_ModelDecideButtonUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_GameObject_ModelDecideButtonUI"),
		CModelDecideButtonUI::Create(m_pDevice, m_pContext, OBJECTID::MODELDECIDEBUTTON_UI))))
		return E_FAIL;

	/* For.Prototype_GameObject_Mannequin */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_GameObject_Mannequin"),
		CMannequin::Create(m_pDevice, m_pContext, OBJECTID::MANNEQUIN))))
		return E_FAIL;

	m_strMessage = TEXT("로딩이 완료되었습니다..");
	
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Tutorial()
{
	m_fLoadingProgress += 0.4f;
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile0.jpg"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_SkillSlotUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_Component_Texture_SkillSlotUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/SkillSlot/SkillSlot%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_TimerPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_Component_Texture_TimerPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Timer/Timer%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_DecimalUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_Component_Texture_DecimalUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Decimal/Decimal%d.png"), 20))))
		return E_FAIL;
	
	/* For.Prototype_Component_Texture_MissionAlertPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_Component_Texture_MissionAlertPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MissionAlert/MissionAlert.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_AttackTypePanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_Component_Texture_AttackTypePanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/AttackType/AttackType%d.png"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_ComboKOPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_Component_Texture_ComboKOPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/ComboKO/ComboKOPanel%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_KOUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_Component_Texture_KOUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/ComboKO/KO_%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_WinPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_Component_Texture_WinPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Win/Win.png"), 1))))
		return E_FAIL;

	m_fLoadingProgress += 0.3f;
	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp")))))
		return E_FAIL;

	///* For.Prototype_Component_VIBuffer_Cube */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	m_fLoadingProgress += 0.3f;
	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

	/* For.Prototype_GameObject_Effect_SkillCoolDown */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Effect_SkillCoolDown"),
		CEffect_CoolDown::Create(m_pDevice, m_pContext, OBJECTID::EFFECT))))
		return E_FAIL;

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");
	/* For.Prototype_GameObject_SkyBox */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_SkyBox"),
		CSkyBox::Create(m_pDevice, m_pContext, OBJECTID::SKYBOX))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext, OBJECTID::TERRAIN))))
		return E_FAIL;

	/* For.Prototype_GameObject_TutorialMap */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_TutorialMap"),
		CTutorialMap::Create(m_pDevice, m_pContext, OBJECTID::TUTORIAL_MAP))))
		return E_FAIL;

	/* For.Prototype_GameObject_COL_TutorialMap */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_COL_TutorialMap"),
		CCol_TutorialMap::Create(m_pDevice, m_pContext, OBJECTID::TUTORIAL_MAP))))
		return E_FAIL;

	/* For.Prototype_Component_Model_COL_TutorialMap */
	_matrix PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_Component_Model_COL_TutorialMap"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/TutorialMap/COL_TutorialMap.fbx", PreTransformMatrix))))
		return E_FAIL;


	/* For.Prototype_GameObject_WhiteJetsu */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_WhiteJetsu"),
		CWhiteJetsu::Create(m_pDevice, m_pContext, OBJECTID::WHITEJETSU))))
		return E_FAIL;

	/* For.Prototype_GameObject_WoodHand */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_WoodHand"),
		CWoodHand::Create(m_pDevice, m_pContext, OBJECTID::WOODHAND))))
		return E_FAIL;

#pragma region MONSTER
	/* For.Prototype_GameObject_Bird */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Bird"),
		CBird::Create(m_pDevice, m_pContext, OBJECTID::BIRD))))
		return E_FAIL;

	/* For.Prototype_GameObject_Boxer */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Boxer"),
		CBoxer::Create(m_pDevice, m_pContext, OBJECTID::BOXER))))
		return E_FAIL;

	/* For.Prototype_GameObject_Pajama */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Pajama"),
		CPajama::Create(m_pDevice, m_pContext, OBJECTID::PAJAMA))))
		return E_FAIL;

	/* For.Prototype_GameObject_TestCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_TestCamera"),
		CTestCamera::Create(m_pDevice, m_pContext, OBJECTID::TEST_CAMERA))))
		return E_FAIL;
#pragma endregion

#pragma region CAMERA
	/* For.Prototype_GameObject_MainCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_MainCamera"),
		CMainCamera::Create(m_pDevice, m_pContext, OBJECTID::MAIN_CAMERA))))
		return E_FAIL;

	/* For.Prototype_GameObject_ActionCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_ActionCamera"),
		CActionCamera::Create(m_pDevice, m_pContext, OBJECTID::ACTION_CAEMRA))))
		return E_FAIL;

	/* For.Prototype_GameObject_SkillActionCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_SkillActionCamera"),
		CSkillActionCamera::Create(m_pDevice, m_pContext, OBJECTID::SKILL_ACTION_CAMERA))))
		return E_FAIL;

	/* For.Prototype_GameObject_NPCTalkCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_NPCTalkCamera"),
		CNPCTalkCamera::Create(m_pDevice, m_pContext, OBJECTID::NPC_TALK_CAMERA))))
		return E_FAIL;


#pragma endregion

#pragma region PLAYER

	/* For.Prototype_GameObject_Upper_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Upper_Player"),
		CUpper_Character::Create(m_pDevice, m_pContext, OBJECTID::PLAYER_UPPER))))
		return E_FAIL;

	/* For.Prototype_GameObject_Lower_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Lower_Player"),
		CLower_Character::Create(m_pDevice, m_pContext, OBJECTID::PLAYER_LOWER))))
		return E_FAIL;

	/* For.Prototype_GameObject_Face_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Face_Player"),
		CFace_Character::Create(m_pDevice, m_pContext, OBJECTID::PLAYER_FACE))))
		return E_FAIL;

	/* For.Prototype_GameObject_Head_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Head_Player"),
		CHead_Character::Create(m_pDevice, m_pContext, OBJECTID::PLAYER_HEAD))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Weapon_Player"),
		CWeapon_Character::Create(m_pDevice, m_pContext, OBJECTID::PLAYER_WEAPON))))
		return E_FAIL;

#pragma endregion

#pragma region UI

	/* For.Prototype_GameObject_SkillSlotUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_SkillSlotUI"),
		CSkillSlotUI::Create(m_pDevice, m_pContext, OBJECTID::SKILLSLOT_UI))))
		return E_FAIL;

	/* For.Prototype_GameObject_SkillSlotPanel*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_SkillSlotPanel"),
		CSkillSlotPanel::Create(m_pDevice, m_pContext, OBJECTID::SKILLSLOT_PANEL))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Skill_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_Component_Texture_Skill_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/SkillIcon/Skill_Icon_%d.png"), 10))))
		return E_FAIL;

	/* For.Prototype_GameObject_DecimalUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_DecimalUI"),
		CDecimalUI::Create(m_pDevice, m_pContext, OBJECTID::DECIMAL_UI))))
		return E_FAIL;

	/* For.Prototype_GameObject_TimerPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_TimerPanel"),
		CTimerPanel::Create(m_pDevice, m_pContext, OBJECTID::TIMER_UI))))
		return E_FAIL;

	/* For.Prototype_GameObject_AttackTypePanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_AttackTypePanel"),
		CAttackTypePanel::Create(m_pDevice, m_pContext, OBJECTID::ATTACKTYPE_PANEL))))
		return E_FAIL;

	/* For.Prototype_GameObject_MissionAlertPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_MissionAlertPanel"),
		CMissionAlertPanel::Create(m_pDevice, m_pContext, OBJECTID::MISSIONALERT_PANEL))))
		return E_FAIL;

	/* For.Prototype_GameObject_ComboKOPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_ComboKOPanel"),
		CComboKOPanel::Create(m_pDevice, m_pContext, OBJECTID::COMBOKO_PANEL))))
		return E_FAIL;

	/* For.Prototype_GameObject_KOUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_KOUI"),
		CKOUI::Create(m_pDevice, m_pContext, OBJECTID::KO_UI))))
		return E_FAIL;

	/* For.Prototype_GameObject_DialogUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_DialogUI"),
		CDialogUI::Create(m_pDevice, m_pContext, OBJECTID::DIALOG_UI))))
		return E_FAIL;

	/* For.Prototype_GameObject_WinPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_WinPanel"),
		CWinPanel::Create(m_pDevice, m_pContext, OBJECTID::WIN_UI))))
		return E_FAIL;

#pragma endregion

	//Prototype_GameObject_NPC_Kakashi
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_NPC_Kakashi"),
		CNPC_KaKashi::Create(m_pDevice, m_pContext, OBJECTID::NPC_KAKASHI))))
		return E_FAIL;

	m_strMessage = TEXT("로딩이 완료되었습니다..");
	Sleep(500);
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_KonohaVillage()
{
	m_fLoadingProgress += 0.4f;
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile0.jpg"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_SkillSlotUI*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Texture_SkillSlotUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/SkillSlot/SkillSlot%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_TimerPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Texture_TimerPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Timer/Timer%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_DecimalUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Texture_DecimalUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Decimal/Decimal%d.png"), 20))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MissionAlertPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Texture_MissionAlertPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MissionAlert/MissionAlert.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_AttackTypePanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Texture_AttackTypePanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/AttackType/AttackType%d.png"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_ComboKOPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Texture_ComboKOPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/ComboKO/ComboKOPanel%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_KOUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Texture_KOUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/ComboKO/KO_%d.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_WinPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Texture_WinPanel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Win/Win.png"), 1))))
		return E_FAIL;

	m_fLoadingProgress += 0.3f;
	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp")))))
		return E_FAIL;

	///* For.Prototype_Component_VIBuffer_Cube */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	m_fLoadingProgress += 0.3f;
	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

	/* For.Prototype_GameObject_Effect_SkillCoolDown */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Effect_SkillCoolDown"),
		CEffect_CoolDown::Create(m_pDevice, m_pContext, OBJECTID::EFFECT))))
		return E_FAIL;

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");
	/* For.Prototype_GameObject_SkyBox */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_SkyBox"),
		CSkyBox::Create(m_pDevice, m_pContext, OBJECTID::SKYBOX))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext, OBJECTID::TERRAIN))))
		return E_FAIL;

#pragma region MAP

	/* For.Prototype_GameObject_KonohaVillage */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_KonohaVillage"),
		CKonohaVillage::Create(m_pDevice, m_pContext, OBJECTID::KONOHA_VILLAGE))))
		return E_FAIL;

	/* For.Prototype_GameObject_COL_KonohaVillage */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_COL_KonohaVillage"),
		CCol_KonohaVillage::Create(m_pDevice, m_pContext, OBJECTID::KONOHA_VILLAGE))))
		return E_FAIL;

#pragma endregion

	/* For.Prototype_GameObject_WhiteJetsu */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_WhiteJetsu"),
		CWhiteJetsu::Create(m_pDevice, m_pContext, OBJECTID::WHITEJETSU))))
		return E_FAIL;

	/* For.Prototype_GameObject_WoodHand */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_WoodHand"),
		CWoodHand::Create(m_pDevice, m_pContext, OBJECTID::WOODHAND))))
		return E_FAIL;

#pragma region MONSTER
	/* For.Prototype_GameObject_Bird */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Bird"),
		CBird::Create(m_pDevice, m_pContext, OBJECTID::BIRD))))
		return E_FAIL;

	/* For.Prototype_GameObject_Boxer */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Boxer"),
		CBoxer::Create(m_pDevice, m_pContext, OBJECTID::BOXER))))
		return E_FAIL;

	/* For.Prototype_GameObject_Pajama */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Pajama"),
		CPajama::Create(m_pDevice, m_pContext, OBJECTID::PAJAMA))))
		return E_FAIL;

	/* For.Prototype_GameObject_TestCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_TestCamera"),
		CTestCamera::Create(m_pDevice, m_pContext, OBJECTID::TEST_CAMERA))))
		return E_FAIL;

#pragma endregion

#pragma region CAMERA
	/* For.Prototype_GameObject_MainCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_MainCamera"),
		CMainCamera::Create(m_pDevice, m_pContext, OBJECTID::MAIN_CAMERA))))
		return E_FAIL;

	/* For.Prototype_GameObject_ActionCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_ActionCamera"),
		CActionCamera::Create(m_pDevice, m_pContext, OBJECTID::ACTION_CAEMRA))))
		return E_FAIL;

	/* For.Prototype_GameObject_SkillActionCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_SkillActionCamera"),
		CSkillActionCamera::Create(m_pDevice, m_pContext, OBJECTID::SKILL_ACTION_CAMERA))))
		return E_FAIL;

	/* For.Prototype_GameObject_NPCTalkCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_NPCTalkCamera"),
		CNPCTalkCamera::Create(m_pDevice, m_pContext, OBJECTID::NPC_TALK_CAMERA))))
		return E_FAIL;


#pragma endregion

#pragma region PLAYER

	/* For.Prototype_GameObject_Upper_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Upper_Player"),
		CUpper_Character::Create(m_pDevice, m_pContext, OBJECTID::PLAYER_UPPER))))
		return E_FAIL;

	/* For.Prototype_GameObject_Lower_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Lower_Player"),
		CLower_Character::Create(m_pDevice, m_pContext, OBJECTID::PLAYER_LOWER))))
		return E_FAIL;

	/* For.Prototype_GameObject_Face_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Face_Player"),
		CFace_Character::Create(m_pDevice, m_pContext, OBJECTID::PLAYER_FACE))))
		return E_FAIL;

	/* For.Prototype_GameObject_Head_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Head_Player"),
		CHead_Character::Create(m_pDevice, m_pContext, OBJECTID::PLAYER_HEAD))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Weapon_Player"),
		CWeapon_Character::Create(m_pDevice, m_pContext, OBJECTID::PLAYER_WEAPON))))
		return E_FAIL;

#pragma endregion

#pragma region UI

	/* For.Prototype_GameObject_SkillSlotUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_SkillSlotUI"),
		CSkillSlotUI::Create(m_pDevice, m_pContext, OBJECTID::SKILLSLOT_UI))))
		return E_FAIL;

	/* For.Prototype_GameObject_SkillSlotPanel*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_SkillSlotPanel"),
		CSkillSlotPanel::Create(m_pDevice, m_pContext, OBJECTID::SKILLSLOT_PANEL))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Skill_Icon */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Texture_Skill_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/SkillIcon/Skill_Icon_%d.png"), 10))))
		return E_FAIL;

	/* For.Prototype_GameObject_DecimalUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_DecimalUI"),
		CDecimalUI::Create(m_pDevice, m_pContext, OBJECTID::DECIMAL_UI))))
		return E_FAIL;

	/* For.Prototype_GameObject_TimerPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_TimerPanel"),
		CTimerPanel::Create(m_pDevice, m_pContext, OBJECTID::TIMER_UI))))
		return E_FAIL;

	/* For.Prototype_GameObject_AttackTypePanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_AttackTypePanel"),
		CAttackTypePanel::Create(m_pDevice, m_pContext, OBJECTID::ATTACKTYPE_PANEL))))
		return E_FAIL;

	/* For.Prototype_GameObject_MissionAlertPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_MissionAlertPanel"),
		CMissionAlertPanel::Create(m_pDevice, m_pContext, OBJECTID::MISSIONALERT_PANEL))))
		return E_FAIL;

	/* For.Prototype_GameObject_ComboKOPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_ComboKOPanel"),
		CComboKOPanel::Create(m_pDevice, m_pContext, OBJECTID::COMBOKO_PANEL))))
		return E_FAIL;

	/* For.Prototype_GameObject_KOUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_KOUI"),
		CKOUI::Create(m_pDevice, m_pContext, OBJECTID::KO_UI))))
		return E_FAIL;

	/* For.Prototype_GameObject_DialogUI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_DialogUI"),
		CDialogUI::Create(m_pDevice, m_pContext, OBJECTID::DIALOG_UI))))
		return E_FAIL;

	/* For.Prototype_GameObject_WinPanel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_WinPanel"),
		CWinPanel::Create(m_pDevice, m_pContext, OBJECTID::WIN_UI))))
		return E_FAIL;

#pragma endregion

	//Prototype_GameObject_NPC_Kakashi
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_NPC_Kakashi"),
		CNPC_KaKashi::Create(m_pDevice, m_pContext, OBJECTID::NPC_KAKASHI))))
		return E_FAIL;

	/* For.Prototype_Component_Model_KonohaVillage */
	_matrix PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Model_KonohaVillage"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, TEXT("../Bin/Resources/Models/KonohaVillage/KonohaVillage.bin"), PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_COL_KonohaVillage */
	PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Model_COL_KonohaVillage"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/KonohaVillage/COL_KonohaVillage.fbx", PreTransformMatrix))))
		return E_FAIL;

	m_strMessage = TEXT("로딩이 완료되었습니다..");
	Sleep(500);
	m_isFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
