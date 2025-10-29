#include "Level_Edit.h"
#include "GameInstance.h"

#include "TestCamera.h"
#include "EditCamera.h"
#include "Terrain.h"
#include "Mannequin.h"
#include "Props.h"
#include "KonohaVillage.h"
#include "TutorialMap.h"

#include "Pooling.h"
#include "NavigationMesh.h"

#include "MapConverter.h"
#include "Tree.h"
#include "Gate.h"

/* 부모의 멤버 변수 세팅은 부모에서 처리해야 한다.. */
CLevel_Edit::CLevel_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID) :
    CLevel { pDevice, pContext, ENUM_CLASS(eLevelID) }
{
}

HRESULT CLevel_Edit::Initialize()
{
    /* IMGUI의 모든 GUI를 활성화 해주도록 한다. */

    /* DX11은 Dx9과 다르게 기본적인 광원 정보를 저장해주지 않는다. */
    /* 광원은 후처리 연산이 훨씬 유리하다고 하심 */
    /* Desc 세팅해서 라이트 매니저에 넣어줌. */
    /* DX11은 Dx9과 다르게 기본적인 광원 정보를 저장해주지 않는다. */
    /* 광원은 후처리 연산이 훨씬 유리하다고 하심 */
    /* Desc 세팅해서 라이트 매니저에 넣어줌. */
    LIGHT_DESC			LightDesc{};

    LightDesc.eType = LIGHT::DIRECTIONAL;
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
    LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    SHADOW_LIGHT_DESC		ShadowDesc{};
    ShadowDesc.vEye = _float4(2.9f, 87.1f, -91.9f, 1.f);
    ShadowDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    ShadowDesc.fFovy = XMConvertToRadians(60.0f);
    ShadowDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
    ShadowDesc.fNear = 0.1f;
    ShadowDesc.fFar = 3000.f;

    if (FAILED(m_pGameInstance->Ready_Shadow_Light(ShadowDesc)))
        return E_FAIL;

    if (FAILED(Ready_Prototypes()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;
     
    if (FAILED(Ready_Layer_Props(TEXT("Layer_Props"))))
        return E_FAIL;


    m_pGameInstance->Set_Visible_All_IMGUI(true);

    m_MapConverter = CMapConverter::Create(m_pDevice, m_pContext);

	m_pEditorNavigation = CNavigation::Create(m_pDevice, m_pContext, TEXT("../CONVERT/Navi/Navigation_Edit.dat"));
    if (nullptr == m_pEditorNavigation)
        return E_FAIL;

    return S_OK;
}

void CLevel_Edit::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_F3))
        m_pGameInstance->Set_Visible_All_IMGUI(true);

    if (m_pGameInstance->Key_Down(DIK_F4))
        m_pGameInstance->Set_Visible_All_IMGUI(false);


    m_pEditorNavigation->Update(XMMatrixIdentity());

    Editor_GUI();

    Edit_NavPoint();
    Move_Props();
}

HRESULT CLevel_Edit::Render()
{
    SetWindowText(g_hWnd, TEXT("EDIT LEVEL"));

    m_pEditorNavigation->Render();

    return S_OK;
}

void CLevel_Edit::Move_Props()
{
    if (!m_IsPickingOn)
        return;

    if (m_pSelectedObject != nullptr && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON))
    {
        _float3 vOut;
        if (m_pGameInstance->Picking(ENUM_CLASS(LEVEL::EDIT), &vOut))
            m_pSelectedTransform->Set_State(STATE::POSITION, XMVectorSet(vOut.x, vOut.y, vOut.z, 1.f));
    }
}

void CLevel_Edit::Edit_NavPoint()
{
    if (!m_IsNavPickingOn)
        return;

    if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LBUTTON))
    {
        _float3 vOut = { 0.f, 0.f, 0.f };
        _float3 vNearPoint = { 0.f, 0.f, 0.f };
        if (m_pGameInstance->Picking(ENUM_CLASS(LEVEL::EDIT), &vOut))
        {
            /* 가까운 포인트가 있다면 가까운 포인트로 세팅*/
            if (m_pEditorNavigation->IsNearPoint(vOut, 2.f, &vNearPoint))
                m_vNavPoints[m_iCurrentNavIdx++] = vNearPoint;
            /* 그게 아니라면 그냥 세팅 */
            else
                m_vNavPoints[m_iCurrentNavIdx++] = vOut;

            if (m_iCurrentNavIdx >= ENUM_CLASS(NAVI_POINT::END))
            {
                /* 셀 만들어서 던지고 초기화 */
				m_pEditorNavigation->Create_Cells(
                    XMVectorSet(m_vNavPoints[0].x, m_vNavPoints[0].y, m_vNavPoints[0].z, 1.f),
					XMVectorSet(m_vNavPoints[1].x, m_vNavPoints[1].y, m_vNavPoints[1].z, 1.f),
					XMVectorSet(m_vNavPoints[2].x, m_vNavPoints[2].y, m_vNavPoints[2].z, 1.f));
                m_iCurrentNavIdx = 0;
            }

        }
    }
    
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL) && m_pGameInstance->Key_Down(DIK_Z))
        m_pEditorNavigation->Delete_FinalCell();
}

HRESULT CLevel_Edit::Ready_Prototypes()
{
#pragma region MODEL_FBX
    _matrix			PreTransformMatrix = XMMatrixIdentity();


#pragma region PLAYER_HEAD
    ///* 얼굴 3종*/
    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Face_Player_Bandage"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Face_Player.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Face_Player_Bandage"));

    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Face_Player_Default"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Face_Player_Default.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Face_Player_Default"));


    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Face_Player_Mask"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Face_Player_Mask.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Face_Player_Mask"));

#pragma endregion


#pragma region PLAYER_FACE
    /* 머리 */
    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Head_Player"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Head_Player.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Head_Player"));

    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Head_Player_Allback"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Head_Player_Allback.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Head_Player_Allback"));

    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Head_Player_Hat"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Head_Player_Hat.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Head_Player_Hat"));

    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Head_Player_Sanbal"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Head_Player_Sanbal.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Head_Player_Sanbal"));

    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Head_Player_SnowMan"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Head_Player_SnowMan.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Head_Player_SnowMan"));
#pragma endregion


#pragma region PLAYER_LOWER
    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Lower_Player"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Lower_Player.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Lower_Player"));

    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Lower_Ambu"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Lower_Player_Ambu.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Lower_Ambu"));

    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Lower_Durumari"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Lower_Player_Durumari.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Lower_Durumari"));

    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Lower_Orochimaru"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Lower_Player_Orochimaru.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Lower_Orochimaru"));

    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Lower_Sasuke"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Lower_Player_Sasuke.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Lower_Sasuke"));

    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Lower_Shoichi"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Lower_Player_Shoichi.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Lower_Shoichi"));

    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Lower_Stone"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Lower_Player_Stone.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Lower_Stone"));

#pragma endregion


#pragma region PLAYER_ONECLOTH

    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_OneCloth_Akastuki"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/OneCloth_Player_Akastuki.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_OneCloth_Akastuki"));

    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_OneCloth_Player_Beach"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/OneCloth_Player_Beach.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_OneCloth_Player_Beach"));

    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_OneCloth_Player_SnowMan"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/OneCloth_Player_SnowMan.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_OneCloth_Player_SnowMan"));

    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_OneCloth_Player_Training"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/OneCloth_Player_Training.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_OneCloth_Player_Training"));

    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_OneCloth_Player_Tuxedo"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/OneCloth_Player_Tuxedo.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_OneCloth_Player_Tuxedo"));


#pragma endregion


//#pragma region PLAYER_UPPER
//
//    PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
//    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Upper_Player"),
//        CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Upper_Player.fbx", PreTransformMatrix))))
//        return E_FAIL;
//    m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Upper_Player"));
//
//    PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
//    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Upper_Player_Ambu"),
//        CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Upper_Player_Ambu.fbx", PreTransformMatrix))))
//        return E_FAIL;
//    m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Upper_Player_Ambu"));
//
//    PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
//    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Upper_Player_Konoha"),
//        CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Upper_Player_Konoha.fbx", PreTransformMatrix))))
//        return E_FAIL;
//    m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Upper_Player_Konoha"));
//
//    PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
//    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Upper_Player_Racer"),
//        CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Upper_Player_Racer.fbx", PreTransformMatrix))))
//        return E_FAIL;
//    m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Upper_Player_Racer"));
//
//    PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
//    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Upper_Player_Shoichi"),
//        CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Upper_Player_Shoichi.fbx", PreTransformMatrix))))
//        return E_FAIL;
//    m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Upper_Player_Shoichi"));
//
//    PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
//    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Upper_Player_Stone"),
//        CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Upper_Player_Stone.fbx", PreTransformMatrix))))
//        return E_FAIL;
//    m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Upper_Player_Stone"));
//
//    PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
//    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Upper_Player_Yuljung"),
//        CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Upper_Player_Yuljung.fbx", PreTransformMatrix))))
//        return E_FAIL;
//    m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Upper_Player_Yuljung"));
//#pragma endregion
//
//#pragma region PLAYER_WEAPON
//    PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
//    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Weapon_Player"),
//        CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/Player/Weapon_Player.fbx", PreTransformMatrix))))
//        return E_FAIL;
//    m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Weapon_Player"));
//#pragma endregion 


    ///* For.Prototype_Component_Model_SkyBox */
    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_SkyBox"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Client/Bin/Resources/Models/SkyBox/SkyBox.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_SkyBox"));
    ///* 목둔 박수 */
    ///* For.Prototype_Component_Model_WoodHand_L */
    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(270.0f)) * XMMatrixTranslation(-2.65f, 0.f, 0.f);
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_WoodHand_L"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/WoodHand/WoodHand_L.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_WoodHand_L"));
    ///* For.Prototype_Component_Model_WoodHand_R */
    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixTranslation(2.65f, 0.f, 0.f);
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_WoodHand_R"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/WoodHand/WoodHand_R.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_WoodHand_R"));

    ///* For.Prototype_Component_Model_NPC_Kakashi*/
    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_NPC_Kakashi"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../../Client/Bin/Resources/Models/NPC/NPC_Kakashi.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_NPC_Kakashi"));

    ///* For.Prototype_Component_Model_Kunai*/
    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(90.0f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Kunai"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Client/Bin/Resources/Models/Kunai/Kunai.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Kunai"));

    ///* For.Prototype_Component_Model_Kunai_Diff*/
    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f)) * XMMatrixRotationY(XMConvertToRadians(270.0f));
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Kunai_Diff"),
    //    CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Client/Bin/Resources/Models/Kunai/Kunai2.fbx", PreTransformMatrix))))
    //    return E_FAIL;
    //m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Kunai_Diff"));

    /* For.Prototype_Component_Model_SkyBox */
    PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_1000hand"),
        CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Client/Bin/Resources/Models/1000hand/1000hand.fbx", PreTransformMatrix))))
        return E_FAIL;
    m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_1000hand"));

    /* For.Prototype_Component_Model_SkyBox */
    PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_1000hand_arm"),
        CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Client/Bin/Resources/Models/1000hand/1000hand_arm.fbx", PreTransformMatrix))))
        return E_FAIL;
    m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_1000hand_arm"));
#pragma region MODEL_BINARY


#pragma region COMPONENT
    /* For.Prototype_Component_Pooling*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Pooling"),
        CPooling::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    //PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    ///* For.Prototype_Component_NavigationMesh*/
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_NavigationMesh"),
    //    CNavigationMesh::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/Models/KonohaVillage/NavMesh_KonohaVillage.fbx", PreTransformMatrix))))
    //    return E_FAIL;

#pragma endregion

#pragma region BUFFER 
    /* For.Prototype_Component_VIBuffer_Rect */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_VIBuffer_Rect"),
        CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion

#pragma region SHADER
    /* For.Prototype_Component_Shader_VtxPosTex */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Shader_VtxPosTex"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    /* For.Prototype_Component_Shader_VtxNorTex */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Shader_VtxNorTex"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
        return E_FAIL;

    /* For.Prototype_Component_Shader_VtxMesh */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Shader_VtxMesh"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
        return E_FAIL;

    /* For.Prototype_Component_Shader_VtxAnimMesh */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
        return E_FAIL;

#pragma endregion

#pragma region OBJECT
    /*추후 여기서 게이트, 나무 추가*/

    /* For.Prototype_GameObject_Gate */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_Gate"),
        CGate::Create(m_pDevice, m_pContext, Client::OBJECTID::GATE))))
        return E_FAIL;
    m_MapObjectPrototypeTags.push_back(TEXT("Prototype_GameObject_Gate"));

    /* For.Prototype_GameObject_Tree */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_Tree"),
        CTree::Create(m_pDevice, m_pContext, Client::OBJECTID::TREE))))
        return E_FAIL;
    m_MapObjectPrototypeTags.push_back(TEXT("Prototype_GameObject_Tree"));

    /* For.Prototype_GameObject_TestCamera */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_EditCamera"),
        CEditCamera::Create(m_pDevice, m_pContext, Client::OBJECTID::TEST_CAMERA))))
        return E_FAIL;

    /* For.Prototype_GameObject_Terrain */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_Terrain"),
        CTerrain::Create(m_pDevice, m_pContext, Client::OBJECTID::TERRAIN))))
        return E_FAIL;

    /* For.Prototype_GameObject_Mannequin */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_Mannequin"),
        CMannequin::Create(m_pDevice, m_pContext, Client::OBJECTID::MANNEQUIN))))
        return E_FAIL;

    /* For.Prototype_GameObject_Props */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_Props"),
        CProps::Create(m_pDevice, m_pContext, Client::OBJECTID::PROPS))))
        return E_FAIL;

    /* For.Prototype_GameObject_KonohaVillage */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_KonohaVillage"),
        CKonohaVillage::Create(m_pDevice, m_pContext, Client::OBJECTID::KONOHA_VILLAGE))))
        return E_FAIL;

    /* For.Prototype_GameObject_TutorialMap */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_TutorialMap"),
        CTutorialMap::Create(m_pDevice, m_pContext, Client::OBJECTID::TUTORIAL_MAP))))
        return E_FAIL;

#pragma endregion

    return S_OK;
}

HRESULT CLevel_Edit::Ready_Layer_Camera(const _wstring& strLayerTag)
{
    CEditCamera::EDIT_CAMERA_DESC			CameraDesc{};

    CameraDesc.fFovy = XMConvertToRadians(60.0f);
    CameraDesc.fNear = 0.1f;
    CameraDesc.fFar = 500.f;
    CameraDesc.vEye = _float4(0.f, 30.f, -30.f, 1.f);
    CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    CameraDesc.fSpeedPerSec = 20.f;
    CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
    CameraDesc.fMouseSensitiy = 0.2f;

    m_fCameraSensivity = CameraDesc.fMouseSensitiy;
    m_fCameraSpeedFactor = 1.f;

    m_pEditCamera = static_cast<CEditCamera*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_EditCamera"), &CameraDesc));
    Safe_AddRef(m_pEditCamera);

    m_pGameInstance->Add_Clone_ToLayer(m_pEditCamera, ENUM_CLASS(LEVEL::EDIT), TEXT("Layer_Camera"));

    return S_OK;
}

HRESULT CLevel_Edit::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_KonohaVillage"),
        ENUM_CLASS(LEVEL::EDIT), strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Edit::Ready_Layer_Props(const _wstring& strLayerTag)
{
    ///* 스태틱 오브젝트들 추가. */
    //CProps::PROP_DESC PropDesc;
    //PropDesc.iMeshIdx = 0;
    //PropDesc.iShaderPassIdx = 0;

    //if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_Props"),
    //    ENUM_CLASS(LEVEL::EDIT), strLayerTag, &PropDesc)))
    //    return E_FAIL;

    return S_OK;
}

void CLevel_Edit::Editor_GUI()
{
    ImGui::Begin("Editor");
    
    Scene_Setting();

    ImGui::BeginTabBar("Editor");

    ExportAndImport();
    Map_Editor();
    Model_Editor();
    Effect_Editor();

    ImGui::EndTabBar();
    ImGui::End();
}

void CLevel_Edit::Scene_Setting()
{
    if (ImGui::BeginTabBar("SCENE_SETTING"))
    {
        if (ImGui::BeginTabItem("Scene"))
        {
            if (m_pGameInstance->Key_Down(DIK_F1))
            {
                m_IsPickingOn = !m_IsPickingOn;
            }

            if (m_pGameInstance->Key_Down(DIK_EQUALS))
            {
                m_IsNavPickingOn = !m_IsNavPickingOn;
            }

            if (m_pGameInstance->Key_Down(DIK_F2))
            {
                m_IsCameraOn = !m_IsCameraOn;
                m_pEditCamera->Activate_Camera(m_IsCameraOn);
            }

            if (ImGui::Checkbox("Picking Activate (Press F1 To Toggle)", &m_IsPickingOn)) {}


            if (ImGui::Checkbox("Navigation Activate (Press = To Toggle)", &m_IsNavPickingOn)) {}

			ImGui::Text("CURRENT NAV IDX : %d", m_iCurrentNavIdx);

            if (ImGui::Checkbox("Camera Activate (Press F2 To Toggle)", &m_IsCameraOn))
            {
                m_pEditCamera->Activate_Camera(m_IsCameraOn);
            }

            ImGui::InputFloat("Camera Sensivity", &m_fCameraSensivity);
            ImGui::InputFloat("Camera SpeedFactor", &m_fCameraSpeedFactor);

            if (ImGui::Button("Apply Camera"))
            {
                CEditCamera::EDIT_CAMERA_DESC Desc;
                Desc.fMouseSensitiy = m_fCameraSensivity;
                Desc.fSpeedPerSec = m_fCameraSpeedFactor;

                m_pEditCamera->Set_Desc(&Desc);
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::Dummy(ImVec2{ 0.f, 5.f });
}

void CLevel_Edit::Map_Editor()
{

    /* 맵 제어 */
    if (ImGui::BeginTabItem("Deploy"))
    {
        ImGui::Dummy(ImVec2(0.0f, 10.f));

        ImGui::DragFloat("Mouse Speed", &m_fMouseActionSpeed, 0.02f, 0.1f, 10.0f, "%.3f");
        ImGui::Dummy(ImVec2(0.0f, 10.f));

        ImGui::Separator();

        ImGui::Text("TRANSFORM (Current Selected Object) : ");

        if (nullptr != m_pSelectedObject || nullptr != m_pSelectedObject)
        {
            Show_SelectedObject();
            ImGui::Dummy(ImVec2{ 0.f,10.f });
            Edit_SelectedObject();
        }

        ImGui::Dummy(ImVec2(0.f, 15.f));

        if (ImGui::BeginTabBar("CREATEOBJECT"))
        {
            if (ImGui::BeginTabItem("Create Object"))
            {
                ImGui::Text("Create Objects");

                ImGui::BeginChild("MapObject List", ImVec2(0, 60), true);

                for (size_t i = 0; i < m_MapObjectPrototypeTags.size(); ++i)
                {
                    _string Buffer = m_pGameInstance->ToString(m_MapObjectPrototypeTags[i]);
                    if (ImGui::Selectable(Buffer.c_str(), true)) {
                        // 선택 동작
                        m_iSelectedMapPrototypeTag = i;
                    }
                }

                ImGui::EndChild();

                if (ImGui::Button("Clone"))
                {
                    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDIT), m_MapObjectPrototypeTags[m_iSelectedMapPrototypeTag], ENUM_CLASS(LEVEL::EDIT), m_strLayerMapObjectTag)))
                    {
                        ImGui::OpenPopup("CLONE_FAILED");
                    }
                }

                if (ImGui::BeginPopupModal("CLONE_FAILED", 0, ImGuiWindowFlags_NoResize))
                {
                    Align_Center("Check Tag, LevelIdx");
                    ImGui::Text("Check Tag, LevelIdx");

                    ImGui::Dummy(ImVec2{ 0.f, 10.f });

                    Align_Center("Confirm");
                    if (ImGui::Button("Confirm"))
                        ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();

                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Deployed Object"))
            {
                string strBuffer = "Selected Index: " + to_string(m_iSelectedObjectID);
                ImGui::Text(strBuffer.c_str());

                ImGui::BeginChild(m_pGameInstance->ToString(m_strLayerMapObjectTag).c_str(), ImVec2(0, 150), true);

                size_t iObjectCnt = m_pGameInstance->Get_LayerSize(m_pGameInstance->Get_LevelID(), m_strLayerMapObjectTag);

                for (size_t i = 0; i < iObjectCnt; ++i)
                {
                    CGameObject* pGameObject = m_pGameInstance->Get_GameObject(m_pGameInstance->Get_LevelID(), m_strLayerMapObjectTag, (_uint)i);
                    _uint iObjectID = pGameObject->Get_ObjectID();
                    _string strBuffer = to_string(i) + ". " + (iObjectID == ENUM_CLASS(Client::OBJECTID::TREE) ? "Tree" : "Gate");

                    if (ImGui::Selectable(strBuffer.c_str(), true)) {
                        // 선택 동작
                        m_IsSelectedProps = false;
                        m_iSelectedObjectID = (_uint)i;
                        m_pSelectedObject = pGameObject;
                        m_pSelectedTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(m_pGameInstance->Get_LevelID(), m_strLayerMapObjectTag, Engine::g_strTransformTag, (_uint)i));
                    }
                }

                ImGui::EndChild();

                if (ImGui::Button("Delete Object")) {
                    // 삭제 동작
                    CGameObject* pGameObject = m_pGameInstance->Get_GameObject(m_pGameInstance->Get_LevelID(), m_strLayerPropTag, (_uint)m_iSelectedObjectID);
                    if (pGameObject != nullptr)
                        pGameObject->Set_Dead(true);
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::Dummy(ImVec2(0.f, 15.f));

        if (ImGui::BeginTabBar("MAPEDIT"))
        {
            if (ImGui::BeginTabItem("Create Prop"))
            {
                ImGui::Text("Select Prop Mesh");
                ImGui::SameLine();
                ImGui::Text(("(Current Selected IDX: " + to_string(m_iSelectedMeshNum)).c_str());
                //프롭 생성 여기서 작성
                ImGui::BeginChild("Mesh List", ImVec2(0, 150), true);
                CModel* pModel = static_cast<CModel*>(m_pGameInstance->Get_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Props")));
                for (size_t i = 0; i < pModel->Get_NumMeshes(); ++i)
                {
                    _string Buffer = m_pGameInstance->ToString(pModel->Get_MeshName(i));
                    if (ImGui::Selectable(Buffer.c_str(), true)) {
                        // 선택 동작
                        m_iSelectedMeshNum = i;
                    }
                }

                ImGui::EndChild();

                if (ImGui::Button("Clone"))
                {
                    CProps::PROP_DESC Desc;
                    Desc.iMeshIdx = m_iSelectedMeshNum;
                    Desc.iShaderPassIdx = 0;

                    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_Props"), ENUM_CLASS(LEVEL::EDIT), m_strLayerPropTag, &Desc)))
                    {
                        ImGui::OpenPopup("CLONE_FAILED");
                    }
                }

                if (ImGui::BeginPopupModal("CLONE_FAILED", 0, ImGuiWindowFlags_NoResize))
                {
                    Align_Center("Check Tag, LevelIdx");
                    ImGui::Text("Check Tag, LevelIdx");

                    ImGui::Dummy(ImVec2{ 0.f, 10.f });

                    Align_Center("Confirm");
                    if (ImGui::Button("Confirm"))
                        ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Deployed Props"))
            {
                string strBuffer = "Selected Index: " + to_string(m_iSelectedObjectID);
                ImGui::Text(strBuffer.c_str());

                ImGui::BeginChild(m_pGameInstance->ToString(m_strLayerPropTag).c_str(), ImVec2(0, 150), true);

                size_t iObjectCnt = m_pGameInstance->Get_LayerSize(m_pGameInstance->Get_LevelID(), m_strLayerPropTag);

                for (size_t i = 0; i < iObjectCnt; ++i)
                {
                    CGameObject* pGameObject = m_pGameInstance->Get_GameObject(m_pGameInstance->Get_LevelID(), m_strLayerPropTag, (_uint)i);

                    _uint iMeshIdx = static_cast<CProps*>(pGameObject)->Get_MeshIdx();
                    _uint iShaderPassIdx = static_cast<CProps*>(pGameObject)->Get_ShaderPassIdx();
                    CModel* pModel = static_cast<CModel*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::EDIT), TEXT("Layer_Props"), TEXT("Com_Model"), i));
                    _string strBuffer = to_string(i) + ". Prop Info (Mesh: " + m_pGameInstance->ToString(pModel->Get_MeshName(iMeshIdx)) + ") " + "(Shader Pass: " + to_string(iShaderPassIdx) + ")";

                    if (ImGui::Selectable(strBuffer.c_str(), true)) {
                        // 선택 동작
                        m_IsSelectedProps = true;
                        m_iSelectedObjectID = (_uint)i;
                        m_pSelectedObject = pGameObject;
                        m_pSelectedTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(m_pGameInstance->Get_LevelID(), m_strLayerPropTag, Engine::g_strTransformTag, (_uint)i));
                    }
                }

                ImGui::EndChild();

                if (ImGui::Button("Delete Object")) {
                    // 삭제 동작
                    CGameObject* pGameObject = m_pGameInstance->Get_GameObject(m_pGameInstance->Get_LevelID(), m_strLayerPropTag, (_uint)m_iSelectedObjectID);
                    if (pGameObject != nullptr)
                        pGameObject->Set_Dead(true);
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::Dummy(ImVec2{ 0.f, 10.f });
        
        ImGui::Text("Add Map Meshes");

        if (ImGui::BeginTabBar("MAPMESH"))
        {
            if (ImGui::BeginTabItem("Create Map Meshes"))
            {
                ImGui::BeginChild("Map List", ImVec2(0, 150), true);

                //나뭇잎 마을
                m_pGameInstance->Get_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_KonohaVillage"));
                if (ImGui::Selectable("Prototype_GameObject_KonohaVillage", true)) {
                    // 선택 동작
                    m_strSelectedMapName = TEXT("Prototype_GameObject_KonohaVillage");
                }

                //튜토 맵
                m_pGameInstance->Get_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_TutorialMap"));
                if (ImGui::Selectable("Prototype_GameObject_TutorialMap", true)) {
                    // 선택 동작
                    m_strSelectedMapName = TEXT("Prototype_GameObject_TutorialMap");
                }

                ImGui::EndChild();

                ImGui::EndTabItem();
            }

            if (ImGui::Button("Clone"))
            {
                if (m_pGameInstance->Get_LayerSize(ENUM_CLASS(LEVEL::EDIT), TEXT("Layer_Map")) < 1)
                {
                    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDIT), m_strSelectedMapName, ENUM_CLASS(LEVEL::EDIT), TEXT("Layer_Map"))))
                    {
                        ImGui::OpenPopup("CLONE_FAILED");
                    }
                }

            }

            ImGui::EndTabBar();
        }

        ImGui::EndTabItem();
    }
}

void CLevel_Edit::Show_SelectedObject()
{
    ImGui::PushItemWidth(50);

    for (_uint i = 0; i < ENUM_CLASS(STATE::END); ++i)
    {
        _float4 vDest;
        XMStoreFloat4(&vDest, m_pSelectedTransform->Get_State(static_cast<STATE>(i)));

        string strBuffer = "##";
        switch (i)
        {
        case 0:
            ImGui::Text("RIGHT: ");
            strBuffer += "RIGHT: ";
            break;
        case 1:
            ImGui::Text("UP   : ");
            strBuffer += "UP   : ";
            break;
        case 2:
            ImGui::Text("LOOK : ");
            strBuffer += "LOOK : ";
            break;
        case 3:
            ImGui::Text("POS  : ");
            strBuffer += "POS  : ";
            break;
        default:
            break;
        }

        ImGui::SameLine();
        ImGui::InputFloat((strBuffer + to_string(1)).c_str(), &vDest.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        ImGui::InputFloat((strBuffer + to_string(2)).c_str(), &vDest.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        ImGui::InputFloat((strBuffer + to_string(3)).c_str(), &vDest.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        ImGui::InputFloat((strBuffer + to_string(4)).c_str(), &vDest.w, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
    }

    ImGui::PopItemWidth();
}

void CLevel_Edit::Edit_SelectedObject()
{
    //ImGui::Dummy(ImVec2(0.f, 20.f));
    //ImGui::Separator();

    if (ImGui::BeginTabBar("EDITOBJECT"))
    {
        if (ImGui::BeginTabItem("Scale"))
        {
            ImGui::Dummy(ImVec2{ 0.f, 5.f });
            ImGui::Text("Set Scale By Dragging");

            ImGui::PushItemWidth(80.0f); // 크기 지정 (픽셀 단위)
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 10.0f));

            if (ImGui::DragFloat("##xScaling", &m_vScale.x, m_fMouseActionSpeed * 0.1f,
                0.001f, 100.f, "%.3f"))
            {
                m_pSelectedTransform->Set_Scale(m_vScale.x, m_vScale.y, m_vScale.z);
            }


            ImGui::SameLine();

            ImGui::Dummy(ImVec2{ 10.f, 0.f });
            ImGui::SameLine();
            if (ImGui::DragFloat("##yScaling", &m_vScale.y, m_fMouseActionSpeed * 0.1f,
                0.001f, 100.f, "%.3f"))
            {
                m_pSelectedTransform->Set_Scale(m_vScale.x, m_vScale.y, m_vScale.z);
            }

            ImGui::SameLine();

            ImGui::Dummy(ImVec2{ 10.f, 0.f });
            ImGui::SameLine();
            if (ImGui::DragFloat("##zScaling", &m_vScale.z, m_fMouseActionSpeed * 0.1f,
                0.001f, 100.f, "%.3f"))
            {
                m_pSelectedTransform->Set_Scale(m_vScale.x, m_vScale.y, m_vScale.z);
            }

            ImGui::PopStyleVar();
            ImGui::PopItemWidth();

            ImGui::Dummy(ImVec2{ 0.f, 10.f });
            ImGui::Separator();

            ImGui::Dummy(ImVec2{ 0.f, 5.f });
            ImGui::Text("Set Scale By Input");
            ImGui::PushItemWidth(80.f);

            ImGui::InputFloat("##ScaleX", &m_vScale.x, 0.0f, 0.0f, "%.3f");
            if (m_vScale.x <= 0.001f)
                m_vScale.x = 0.001f;
            ImGui::SameLine();
            ImGui::Dummy(ImVec2{ 10.f, 0.f });
            ImGui::SameLine();

            ImGui::InputFloat("##ScaleY", &m_vScale.y, 0.0f, 0.0f, "%.3f");
            if (m_vScale.y <= 0.001f)
                m_vScale.y = 0.001f;
            ImGui::SameLine();
            ImGui::Dummy(ImVec2{ 10.f, 0.f });
            ImGui::SameLine();

            ImGui::InputFloat("##ScaleZ", &m_vScale.z, 0.0f, 0.0f, "%.3f");
            if (m_vScale.z <= 0.001f)
                m_vScale.z = 0.001f;
            ImGui::SameLine();

            ImGui::PopItemWidth();

            if (ImGui::Button("Apply"))
            {
                m_pSelectedTransform->Set_Scale(m_vScale.x, m_vScale.y, m_vScale.z);
            }
            ImGui::Dummy(ImVec2{ 0.f, 5.f });

            ImGui::Separator();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Rotation"))
        {
            ImGui::Dummy(ImVec2{ 0.f, 5.f });
            ImGui::Text("Set Rotation By Dragging");

            ImGui::PushItemWidth(80.0f); // 크기 지정 (픽셀 단위)
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 10.0f));

            if (ImGui::DragFloat("##xRotation", &m_vDegrees.x, m_fMouseActionSpeed * 0.1f,
                0.f, 360.f, "%.3f"))
            {
                m_pSelectedTransform->Rotation(XMConvertToRadians(m_vDegrees.x), XMConvertToRadians(m_vDegrees.y), XMConvertToRadians(m_vDegrees.z));
            }
            ImGui::SameLine();

            ImGui::Dummy(ImVec2{ 10.f, 0.f });

            ImGui::SameLine();
            if (ImGui::DragFloat("##yRotation", &m_vDegrees.y, m_fMouseActionSpeed * 0.1f,
                0.f, 360.f, "%.3f"))
            {
                m_pSelectedTransform->Rotation(XMConvertToRadians(m_vDegrees.x), XMConvertToRadians(m_vDegrees.y), XMConvertToRadians(m_vDegrees.z));
            }
            ImGui::SameLine();

            ImGui::Dummy(ImVec2{ 10.f, 0.f });

            ImGui::SameLine();
            if (ImGui::DragFloat("##zRotation", &m_vDegrees.z, m_fMouseActionSpeed * 0.1f,
                0.f, 360.f, "%.3f"))
            {
                m_pSelectedTransform->Rotation(XMConvertToRadians(m_vDegrees.x), XMConvertToRadians(m_vDegrees.y), XMConvertToRadians(m_vDegrees.z));
            }

            ImGui::PopStyleVar();
            ImGui::PopItemWidth();

            ImGui::Dummy(ImVec2{ 0.f, 10.f });
            ImGui::Separator();

            ImGui::Dummy(ImVec2{ 0.f, 5.f });
            ImGui::Text("Set Rotation By Input");
            ImGui::PushItemWidth(80.f);

            ImGui::InputFloat("##RotationX", &m_vDegrees.x, 0.0f, 0.0f, "%.3f");
            ImGui::SameLine();
            ImGui::Dummy(ImVec2{ 10.f, 0.f });
            ImGui::SameLine();

            ImGui::InputFloat("##RotationY", &m_vDegrees.y, 0.0f, 0.0f, "%.3f");
            ImGui::SameLine();
            ImGui::Dummy(ImVec2{ 10.f, 0.f });
            ImGui::SameLine();

            ImGui::InputFloat("##RotationZ", &m_vDegrees.z, 0.0f, 0.0f, "%.3f");
            ImGui::SameLine();

            ImGui::PopItemWidth();

            if (ImGui::Button("Apply"))
            {
                m_pSelectedTransform->Rotation(m_vDegrees.x, m_vDegrees.y, m_vDegrees.z);
            }
            ImGui::Dummy(ImVec2{ 0.f, 5.f });

            ImGui::Separator();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Position"))
        {
            ImGui::Dummy(ImVec2{ 0.f, 5.f });
            ImGui::Text("Set Position By Dragging");

            ImGui::PushItemWidth(80.0f); // 크기 지정 (픽셀 단위)
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 10.0f));

            if (ImGui::DragFloat("##xPosition", &m_vPosition.x, m_fMouseActionSpeed * 0.2f,
                FLT_MIN, FLT_MAX, "%.3f"))
            {
                m_pSelectedTransform->Set_State(STATE::POSITION, XMLoadFloat4(&m_vPosition));
            }
            ImGui::SameLine();

            ImGui::Dummy(ImVec2{ 10.f, 0.f });

            ImGui::SameLine();
            if (ImGui::DragFloat("##yPosition", &m_vPosition.y, m_fMouseActionSpeed * 0.2f,
                FLT_MIN, FLT_MAX, "%.3f"))
            {
                m_pSelectedTransform->Set_State(STATE::POSITION, XMLoadFloat4(&m_vPosition));
            }
            ImGui::SameLine();

            ImGui::Dummy(ImVec2{ 10.f, 0.f });

            ImGui::SameLine();
            if (ImGui::DragFloat("##zPosition", &m_vPosition.z, m_fMouseActionSpeed * 0.2f,
                FLT_MIN, FLT_MAX, "%.3f"))
            {
                m_pSelectedTransform->Set_State(STATE::POSITION, XMLoadFloat4(&m_vPosition));
            }

            ImGui::PopStyleVar();
            ImGui::PopItemWidth();

            ImGui::Dummy(ImVec2{ 0.f, 10.f });
            ImGui::Separator();

            ImGui::Dummy(ImVec2{ 0.f, 5.f });
            ImGui::Text("Set Position By Input");
            ImGui::PushItemWidth(80.f);

            ImGui::InputFloat("##PositionX", &m_vPosition.x, 0.0f, 0.0f, "%.3f");
            ImGui::SameLine();
            ImGui::Dummy(ImVec2{ 10.f, 0.f });
            ImGui::SameLine();

            ImGui::InputFloat("##PositionY", &m_vPosition.y, 0.0f, 0.0f, "%.3f");
            ImGui::SameLine();
            ImGui::Dummy(ImVec2{ 10.f, 0.f });
            ImGui::SameLine();

            ImGui::InputFloat("##PositionZ", &m_vPosition.z, 0.0f, 0.0f, "%.3f");
            ImGui::SameLine();

            ImGui::PopItemWidth();

            if (ImGui::Button("Apply"))
            {
                m_pSelectedTransform->Set_State(STATE::POSITION, XMLoadFloat4(&m_vPosition));
            }
            ImGui::Dummy(ImVec2{ 0.f, 5.f });

            ImGui::Separator();

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void CLevel_Edit::Model_Editor()
{
    /* 애니메이션 제어 */
    if (ImGui::BeginTabItem("Model"))
    {
        ImGui::EndTabItem();
    }
}

void CLevel_Edit::Effect_Editor()
{
    if (ImGui::BeginTabItem("Effect"))
    {
        ImGui::EndTabItem();
    }
}

void CLevel_Edit::ExportAndImport()
{
    if (ImGui::BeginTabItem("Export"))
    {
        ImGui::Dummy(ImVec2{ 0.f, 10.f });

        if (ImGui::BeginTabBar("EXPORTANDIMPONT"))
        {
            if (ImGui::BeginTabItem("Impot Map Data"))
            {
                ImGui::Text("Input Map File Path: ");

                ImGui::SameLine();
                ImGui::InputText("##Input_LoadMapPath", m_szLoadMapFileName, IM_ARRAYSIZE(m_szLoadMapFileName));

                if (ImGui::Button("Import Maps")) {
                    /* SAVE MAP */
                    m_MapConverter->Import_MapFiles(m_pGameInstance->ToWstring(m_szLoadMapFileName).c_str(), LEVEL::EDIT, LEVEL::EDIT);
                    ImGui::OpenPopup("IMPORT_MAPS_DONE");
                }

                if (ImGui::BeginPopupModal("IMPORT_MAPS_DONE", 0, ImGuiWindowFlags_NoResize))
                {
                    Align_Center("Import Map has Done");
                    ImGui::Text("Import Map has Done");

                    ImGui::Dummy(ImVec2{ 0.f, 10.f });

                    Align_Center("Confirm");
                    if (ImGui::Button("Confirm"))
                        ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Export Map Data "))
            {
                ImGui::Text("Save Map Datas. in \"Layer_Props\", \"Layer_Map\", \"Layer_MapObjects\".");
                ImGui::Text("DONT FORGET TO SAVE MAP DATAS BEFORE QUIT.");

                ImGui::Dummy(ImVec2(0.0f, 20.0f));
                ImGui::Text("Input Save File Path: ");

                ImGui::SameLine();
                ImGui::InputText("##Input_MapPath", m_szMapSavePath, IM_ARRAYSIZE(m_szMapSavePath));

                ImGui::Text("Input Save Map Name: ");

                ImGui::SameLine();
                ImGui::InputText("##Input_MapName", m_szMapFileName, IM_ARRAYSIZE(m_szMapFileName));

                if (ImGui::Button("Export Maps")) {
                    /* SAVE MAP */
                    m_MapConverter->Export_MapFiles(m_pGameInstance->ToWstring(m_szMapFileName).c_str(), m_pGameInstance->ToWstring(m_szMapSavePath).c_str(), LEVEL::EDIT);
                    ImGui::OpenPopup("EXPORT_MAPS_DONE");
                }

                if (ImGui::BeginPopupModal("EXPORT_MAPS_DONE", 0, ImGuiWindowFlags_NoResize))
                {
                    Align_Center("Export Map has Done");
                    ImGui::Text("Export Map has Done");

                    ImGui::Dummy(ImVec2{ 0.f, 10.f });

                    Align_Center("Confirm");
                    if (ImGui::Button("Confirm"))
                        ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Export Navgiation"))
            {
                ImGui::Text("Save Navigations. DONT FORGET TO SAVE Navigation DATAS BEFORE QUIT.");

                if (ImGui::Button("Export Naviations")) {
                    m_pEditorNavigation->Save_NavigationData(TEXT("../CONVERT/Navi/Editor_Navigation.dat"));
                    ImGui::OpenPopup("EXPORT_NAVIGATION_DONE");
                }

                if (ImGui::BeginPopupModal("EXPORT_NAVIGATION_DONE", 0, ImGuiWindowFlags_NoResize))
                {
                    Align_Center("Export NAVIGATION has Done");
                    ImGui::Text("Export NAVIGATION has Done");

                    ImGui::Dummy(ImVec2{ 0.f, 10.f });

                    Align_Center("Confirm");
                    if (ImGui::Button("Confirm"))
                        ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Export FBX"))
            {
                ImGui::Text("Convert FBX Model Files to Custom Binary");
                ImGui::Text("(Only Converts Prototype Models In Edit Level)");
                ImGui::Dummy(ImVec2(0.0f, 20.0f));
                ImGui::Text("Input Save File Path: ");

                ImGui::SameLine();
                ImGui::InputText("##Input_Path", m_szModelSavePath, IM_ARRAYSIZE(m_szModelSavePath));

                if (ImGui::Button("Export FBXs")) {

                    size_t iPrototypeCnt = m_ModelPrototypeTags.size();

                    for (size_t i = 0; i < iPrototypeCnt; ++i)
                    {
                        CModel* pModelCom = dynamic_cast<CModel*>(m_pGameInstance->Get_Prototype(m_pGameInstance->Get_LevelID(), m_ModelPrototypeTags[i]));
                        if (pModelCom != nullptr)
                        {
                            pModelCom->Save_Model_ToBinary(m_szModelSavePath);
                        }
                    }
                    ImGui::OpenPopup("CONVERT_DONE");
                }

                if (ImGui::BeginPopupModal("CONVERT_DONE", 0, ImGuiWindowFlags_NoResize))
                {
                    Align_Center("Convert has Done");
                    ImGui::Text("Convert has Done");

                    ImGui::Dummy(ImVec2{ 0.f, 10.f });

                    Align_Center("Confirm");
                    if (ImGui::Button("Confirm"))
                        ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }
                ImGui::Separator();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
        ImGui::EndTabItem();
    }
}

void CLevel_Edit::Align_Center(const _char* pText)
{
    ImVec2 WindowSize = ImGui::GetWindowSize();
    _float TextSize = ImGui::CalcTextSize(pText).x + ImGui::GetStyle().FramePadding.x * 2.0f;

    ImGui::SetCursorPosX((WindowSize.x - TextSize) * 0.5f);
}

CLevel_Edit* CLevel_Edit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
    CLevel_Edit* pInstance = new CLevel_Edit(pDevice, pContext, eLevelID);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Create Failed: Level_Edit");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Edit::Free()
{
    __super::Free();

    Safe_Release(m_pEditCamera);
    Safe_Release(m_MapConverter);
    Safe_Release(m_pEditorNavigation);
}
