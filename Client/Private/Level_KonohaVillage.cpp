#include "Level_KonohaVillage.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "MainCamera.h"
#include "TestCamera.h"
#include "ActionCamera.h"
#include "SkillActionCamera.h"
#include "NPCTalkCamera.h"

#include "TimerPanel.h"
#include "SkillSlotPanel.h"
#include "AttackTypePanel.h"
#include "ComboKOPanel.h"
#include "DialogUI.h"
#include "MissionAlertPanel.h"
#include "WinPanel.h"
#include "CutScenePanel.h"

#include "Player.h"

CLevel_KonohaVillage::CLevel_KonohaVillage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
	, m_pGameManager{ CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}

HRESULT CLevel_KonohaVillage::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_StaticObjects(TEXT("Layer_StaticObjects"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_NPC(TEXT("Layer_NPC"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(m_pGameManager->Change_Camera(LEVEL::KONOHA_VILLAGE, TEXT("CutScene_Camera"))))
		return E_FAIL;

	m_pGameManager->Get_PlayerPtr()->Change_Skills();

	return S_OK;
}

void CLevel_KonohaVillage::Update(_float fTimeDelta)
{
	/* 디버깅 용 코드*/
	//m_pGameManager->OnTrigger(TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_02_CLEAR);
	///**/
#ifdef _DEBUG
	if (m_pGameInstance->Key_Down(DIK_0))
	{
		m_pGameManager->Change_Camera(LEVEL::KONOHA_VILLAGE, TEXT("Test_Camera"));
	}

	if (m_pGameInstance->Key_Down(DIK_9))
	{
		m_pGameManager->Change_Camera(LEVEL::KONOHA_VILLAGE, TEXT("Main_Camera"));
	}

	if (m_pGameInstance->Key_Down(DIK_F3))
	{
		m_pGameInstance->Set_Visible_IMGUI(true, ENUM_CLASS(IMGUI_VISIBLE::GAMEINFO));
		m_pGameInstance->Set_Visible_IMGUI(true, ENUM_CLASS(IMGUI_VISIBLE::OBJECT_INSPECTOR));
	}

	if (m_pGameInstance->Key_Down(DIK_F4))
	{
		m_pGameInstance->Set_Visible_IMGUI(false, ENUM_CLASS(IMGUI_VISIBLE::GAMEINFO));
		m_pGameInstance->Set_Visible_IMGUI(false, ENUM_CLASS(IMGUI_VISIBLE::OBJECT_INSPECTOR));
	}
#endif

#pragma region COLLISION
	m_pGameManager->Check_Collision(TEXT("Player_Attack"), TEXT("Monster_Body"), COLLISION_TYPE::MONSTER);
	m_pGameManager->Check_Collision(TEXT("Player_Skill"), TEXT("Monster_Body"), COLLISION_TYPE::MONSTER);
	m_pGameManager->Check_Collision(TEXT("Monster_Attack"), TEXT("Player_Body"), COLLISION_TYPE::PLAYER);
	m_pGameManager->Check_Collision(TEXT("Monster_Skill"), TEXT("Player_Body"), COLLISION_TYPE::PLAYER);
	m_pGameManager->Check_Collision(TEXT("Player_Body"), TEXT("TriggerBox"), COLLISION_TYPE::TRIGGER);
#pragma endregion

#pragma region TRIGGER_HANDLE
	if (TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_01 == m_pGameManager->Get_CurrentTrigger())
		//&& (0 == m_pGameInstance->Get_LayerSize(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Layer_Monster"))))
	{
		m_pGameManager->OnTrigger(TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_01_CLEAR);
	}

	if (TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_02 == m_pGameManager->Get_CurrentTrigger())
		//&& (0 == m_pGameInstance->Get_LayerSize(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Layer_Monster"))))
	{
		m_pGameManager->OnTrigger(TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_02_CLEAR);
	}

	if (TRIGGER_TYPE::KONOHA_VILLAGE_BOSS_CUTSCENE_END == m_pGameManager->Get_CurrentTrigger())
		//&& (0 == m_pGameInstance->Get_LayerSize(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Layer_Monster"))))
	{
		m_pGameManager->WinPanel_Start_FadeIn();
		m_fTimeAcc += fTimeDelta;

		if (m_fTimeAcc >= 3.f)
			m_pGameManager->OnTrigger(TRIGGER_TYPE::CLEAR);
	}

#pragma endregion
	
	m_pGameManager->Update_Collision();

	m_pGameInstance->Check_GeometryPicking();
	m_pGameInstance->Check_GeometryCollision();
}

HRESULT CLevel_KonohaVillage::Render()
{
	SetWindowText(g_hWnd, TEXT("나뭇잎 마을"));
	return S_OK;
}

HRESULT CLevel_KonohaVillage::Ready_Lights()
{
	LIGHT_DESC		LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.4f, 0.4f, 0.4f, 0.4f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 0.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_KonohaVillage::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Terrain"),
		ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag)))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_SkyBox"),
		ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_KonohaVillage::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CTestCamera::TEST_CAMERA_DESC			TestCameraDesc{};

	TestCameraDesc.fFovy = XMConvertToRadians(60.0f);
	TestCameraDesc.fNear = 0.1f;
	TestCameraDesc.fFar = 1000.f;
	TestCameraDesc.vEye = _float4(0.f, 30.f, -30.f, 1.f);
	TestCameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	TestCameraDesc.fSpeedPerSec = 20.f;
	TestCameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	TestCameraDesc.fMouseSensitiy = 0.2f;

	/* 카메라는 게임 매니저에 추가하여 관리한다. */
	if (FAILED(m_pGameManager->Add_Camera(LEVEL::KONOHA_VILLAGE, TEXT("Test_Camera"), static_cast<CCamera*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_TestCamera"), &TestCameraDesc)))))
		return E_FAIL;

	CMainCamera::MAIN_CAMERA_DESC			MainCameraDesc{};

	MainCameraDesc.fFovy = XMConvertToRadians(60.0f);
	MainCameraDesc.fNear = 0.1f;
	MainCameraDesc.fFar = 1000.f;
	MainCameraDesc.vEye = _float4(0.f, 30.f, -30.f, 1.f);
	MainCameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	MainCameraDesc.fSpeedPerSec = 15.f;
	MainCameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	MainCameraDesc.pPlayerTransform = m_pGameManager->Get_PlayerPtr()->Get_Transform();
	/* 카메라는 게임 매니저에 추가하여 관리한다. */
	if (FAILED(m_pGameManager->Add_Camera(LEVEL::KONOHA_VILLAGE, TEXT("Main_Camera"), static_cast<CCamera*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_MainCamera"), &MainCameraDesc)))))
		return E_FAIL;


	CActionCamera::ACTION_CAMERA_DESC		ActionCameraDesc{};
	ActionCameraDesc.fFovy = XMConvertToRadians(60.0f);
	ActionCameraDesc.fNear = 0.1f;
	ActionCameraDesc.fFar = 1000.f;
	ActionCameraDesc.vEye = _float4(0.f, 30.f, -30.f, 1.f);
	ActionCameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	ActionCameraDesc.fSpeedPerSec = 15.f;
	ActionCameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	ActionCameraDesc.pPlayerTransform = m_pGameManager->Get_PlayerPtr()->Get_Transform();

	/* 카메라는 게임 매니저에 추가하여 관리한다. */
	if (FAILED(m_pGameManager->Add_Camera(LEVEL::KONOHA_VILLAGE, TEXT("Action_Camera"), static_cast<CCamera*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_ActionCamera"), &ActionCameraDesc)))))
		return E_FAIL;

	CNPCTalkCamera::NPC_TALK_CAMERA_DESC NPCCameraDesc{};
	NPCCameraDesc.fFovy = XMConvertToRadians(60.0f);
	NPCCameraDesc.fNear = 0.1f;
	NPCCameraDesc.fFar = 1000.f;
	NPCCameraDesc.vEye = _float4(0.f, 30.f, -30.f, 1.f);
	NPCCameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	NPCCameraDesc.fSpeedPerSec = 15.f;
	NPCCameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	NPCCameraDesc.pTargetTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Layer_NPC"), Engine::g_strTransformTag, 0));

	/* 카메라는 게임 매니저에 추가하여 관리한다. */
	if (FAILED(m_pGameManager->Add_Camera(LEVEL::KONOHA_VILLAGE, TEXT("NPC_Talk_Caemra"), static_cast<CCamera*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_NPCTalkCamera"), &NPCCameraDesc)))))
		return E_FAIL;

	CSkillActionCamera::SKILL_ACTION_CAMERA_DESC SkillActionCameraDesc{};
	SkillActionCameraDesc.fFovy = XMConvertToRadians(60.0f);
	SkillActionCameraDesc.fNear = 0.1f;
	SkillActionCameraDesc.fFar = 1000.f;
	SkillActionCameraDesc.vEye = _float4(0.f, 30.f, -30.f, 1.f);
	SkillActionCameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	SkillActionCameraDesc.fSpeedPerSec = 15.f;
	SkillActionCameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	SkillActionCameraDesc.pPlayerTransform = m_pGameManager->Get_PlayerPtr()->Get_Transform();
	SkillActionCameraDesc.eSkillType = SKILL::KAMUI;

	/* 카메라는 게임 매니저에 추가하여 관리한다. */
	if (FAILED(m_pGameManager->Add_Camera(LEVEL::KONOHA_VILLAGE, TEXT("Kamui_Action_Camera"), static_cast<CCamera*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_SkillActionCamera"), &SkillActionCameraDesc)))))
		return E_FAIL;

	SkillActionCameraDesc.eSkillType = SKILL::FIREBALL;

	/* 카메라는 게임 매니저에 추가하여 관리한다. */
	if (FAILED(m_pGameManager->Add_Camera(LEVEL::KONOHA_VILLAGE, TEXT("FireBall_Action_Camera"), static_cast<CCamera*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_SkillActionCamera"), &SkillActionCameraDesc)))))
		return E_FAIL;

	SkillActionCameraDesc.eSkillType = SKILL::BIG_SHARK;

	/* 카메라는 게임 매니저에 추가하여 관리한다. */
	if (FAILED(m_pGameManager->Add_Camera(LEVEL::KONOHA_VILLAGE, TEXT("BigShark_Action_Camera"), static_cast<CCamera*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_SkillActionCamera"), &SkillActionCameraDesc)))))
		return E_FAIL;

	SkillActionCameraDesc.eSkillType = SKILL::CHIDORI;

	/* 카메라는 게임 매니저에 추가하여 관리한다. */
	if (FAILED(m_pGameManager->Add_Camera(LEVEL::KONOHA_VILLAGE, TEXT("Chidori_Action_Camera"), static_cast<CCamera*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_SkillActionCamera"), &SkillActionCameraDesc)))))
		return E_FAIL;

	/* 컷씬 카메라. */
	if (FAILED(m_pGameManager->Add_Camera(LEVEL::KONOHA_VILLAGE, TEXT("CutScene_Camera"), static_cast<CCamera*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_CutSceneCamera"), &MainCameraDesc)))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_KonohaVillage::Ready_Layer_Player(const _wstring& strLayerTag)
{
	for (size_t i = 0; i < 1; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player"),
			ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag, nullptr)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_KonohaVillage::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_KonohaVillage::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	for (_int i = 0; i < 10; ++i)
	{
		m_pGameInstance->Add_GameObject_ToPool(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::KONOHA_VILLAGE),
			TEXT("Prototype_GameObject_Effect_SkillCoolDown"), nullptr)));
	}

	for (_int i = 0; i < 50; ++i)
	{
		m_pGameInstance->Add_GameObject_ToPool(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::KONOHA_VILLAGE),
			TEXT("Prototype_GameObject_Effect_Icon"), nullptr)));
	}

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Explosion"),
		ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_KonohaVillage::Ready_Layer_UI(const _wstring& strLayerTag)
{
	CUIObject::UIOBJECT_DESC Desc = CUIObject::CreateDesc(g_iWinSizeX - 100.f, 65.f, 0.45f, 120, 120, 0, 0.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_TimerPanel"),
		ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag, &Desc)))
		return E_FAIL;

	Desc = CUIObject::CreateDesc(g_iWinSizeX / 2.f + 450, g_iWinSizeY / 2.f + 300, 0.40f, 120, 120, 0, 0.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_SkillSlotPanel"),
		ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag, &Desc)))
		return E_FAIL;

	m_pGameManager->Set_SkillSlotPanel(static_cast<CSkillSlotPanel*>(m_pGameInstance->Get_GameObject(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag, m_pGameInstance->Get_LayerSize(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag) - 1)));


	Desc = CUIObject::CreateDesc(g_iWinSizeX / 2.f + 450, g_iWinSizeY / 2.f + 220, 0.40f, 250, 30, 0, 0.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_AttackTypePanel"),
		ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag, &Desc)))
		return E_FAIL;

	m_pGameManager->Set_AttackTypePanel(static_cast<CAttackTypePanel*>(m_pGameInstance->Get_GameObject(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag, m_pGameInstance->Get_LayerSize(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag) - 1)));


	Desc = CUIObject::CreateDesc(g_iWinSizeX / 2.f + 430.f, g_iWinSizeY / 2.f + 20.f, 0.30f, 300, 100, 0, 30.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_ComboKOPanel"),
		ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag, &Desc)))
		return E_FAIL;

	m_pGameManager->Set_ComboKoPanel(static_cast<CComboKOPanel*>(m_pGameInstance->Get_GameObject(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag, m_pGameInstance->Get_LayerSize(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag) - 1)));

	Desc = CUIObject::CreateDesc(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f - 240, 0.30f, g_iWinSizeX - 250, 50, 0, 0.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_MissionAlertPanel"),
		ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag, &Desc)))
		return E_FAIL;

	m_pGameManager->Set_MissionAlertPanel(static_cast<CMissionAlertPanel*>(m_pGameInstance->Get_GameObject(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag, m_pGameInstance->Get_LayerSize(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag) - 1)));

	Desc = CUIObject::CreateDesc(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f + 150.f, 0.05f, 800.f, 200.f, 0, 0.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_DialogUI"),
		ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag, &Desc)))
		return E_FAIL;

	m_pGameManager->Set_Dialog(static_cast<CDialogUI*>(m_pGameInstance->Get_GameObject(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag, m_pGameInstance->Get_LayerSize(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag) - 1)));

	Desc = CUIObject::CreateDesc(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f, 0.01f, g_iWinSizeX / 2.f, g_iWinSizeY / 2.f, 0, 0.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_WinPanel"),
		ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag, &Desc)))
		return E_FAIL;

	m_pGameManager->Set_WinPanel(static_cast<CWinPanel*>(m_pGameInstance->Get_GameObject(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag, m_pGameInstance->Get_LayerSize(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag) - 1)));

	Desc = CUIObject::CreateDesc(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f, 0.01f, g_iWinSizeX, g_iWinSizeY, 0, 0.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_CutScenePanel"),
		ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag, &Desc)))
		return E_FAIL;

	m_pGameManager->Set_CutScenePanel(static_cast<CCutScenePanel*>(m_pGameInstance->Get_GameObject(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag, m_pGameInstance->Get_LayerSize(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag) - 1)));

	return S_OK;
}

HRESULT CLevel_KonohaVillage::Ready_Layer_StaticObjects(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_KonohaVillage::Ready_Layer_NPC(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_NPC_Kakashi"),
		ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_KonohaVillage::Ready_Layer_Map(const _wstring& strLayerTag)
{
	/* 나뭇잎 마을 */
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_KonohaVillage"),
		ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag)))
		return E_FAIL;

	/* 충돌용 메시 (나뭇잎 마을) */
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_COL_KonohaVillage"),
		ENUM_CLASS(LEVEL::KONOHA_VILLAGE), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

CLevel_KonohaVillage* CLevel_KonohaVillage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_KonohaVillage* pInstance = new CLevel_KonohaVillage(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_KonohaVillage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_KonohaVillage::Free()
{
	__super::Free();

	Safe_Release(m_pGameManager);
}
