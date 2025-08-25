#include "Level_Edit.h"
#include "GameInstance.h"

#include "TestCamera.h"
#include "EditCamera.h"
#include "Terrain.h"
#include "Mannequin.h"
#include "Props.h"

#include "Pooling.h"

/* 부모의 멤버 변수 세팅은 부모에서 처리해야 한다.. */
CLevel_Edit::CLevel_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID) :
    CLevel { pDevice, pContext, ENUM_CLASS(eLevelID) }
{
}

HRESULT CLevel_Edit::Initialize()
{
    /* IMGUI의 모든 GUI를 활성화 해주도록 한다. */

    if (FAILED(Ready_Prototypes()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_StaticObjects(TEXT("Layer_StaticObjects"))))
        return E_FAIL;

    m_pGameInstance->Set_Visible_All_IMGUI(true);

    return S_OK;
}

void CLevel_Edit::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_F3))
        m_pGameInstance->Set_Visible_All_IMGUI(true);

    if (m_pGameInstance->Key_Down(DIK_F4))
        m_pGameInstance->Set_Visible_All_IMGUI(false);

    Editor_GUI();
}

HRESULT CLevel_Edit::Render()
{
    SetWindowText(g_hWnd, TEXT("EDIT LEVEL"));

    return S_OK;
}

HRESULT CLevel_Edit::Ready_Prototypes()
{
#pragma region TEXTURE
    /* For.Prototype_Component_Texture_Terrain */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Texture_Terrain"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Terrain/Tile0.jpg"), 1))))
        return E_FAIL;

#pragma endregion

#pragma region MODEL_FBX
    _matrix			PreTransformMatrix = XMMatrixIdentity();

    /* For.Prototype_Component_Model_Fiona */
    PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Fiona"),
        CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Client/Bin/Resources/Models/Fiona/Fiona.fbx", PreTransformMatrix))))
        return E_FAIL;
    m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Fiona"));

    /* For.Prototype_Component_Model_Props */
    PreTransformMatrix = XMMatrixScalingFromVector(XMVectorSet(0.01f, 0.01f, 0.01f, 0.f));
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Props"),
        CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../../Client/Bin/Resources/Models/Props/Props.fbx", PreTransformMatrix))))
        return E_FAIL;
    m_ModelPrototypeTags.push_back(TEXT("Prototype_Component_Model_Props"));

#pragma endregion

#pragma region MODEL_BINARY

    ///* For.Prototype_Component_Model_Fiona_Binary */
    PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Fiona_Binary"),
        CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, TEXT("../../Client/Bin/Resources/Models/Fiona/Fiona.bin"), PreTransformMatrix))))
        return E_FAIL;
#pragma endregion

#pragma region COMPONENT
    /* For.Prototype_Component_Pooling*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Pooling"),
        CPooling::Create(m_pDevice, m_pContext))))
        return E_FAIL;

#pragma endregion

#pragma region BUFFER 
    /* For.Prototype_Component_VIBuffer_Terrain */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_VIBuffer_Terrain"),
        CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Terrain/Height1.bmp")))))
        return E_FAIL;

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
#pragma endregion

#pragma region OBJECT
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
#pragma endregion

    return S_OK;
}

HRESULT CLevel_Edit::Ready_Layer_Camera(const _wstring& strLayerTag)
{
    CEditCamera::EDIT_CAMERA_DESC			CameraDesc{};

    CameraDesc.fFovy = XMConvertToRadians(60.0f);
    CameraDesc.fNear = 0.1f;
    CameraDesc.fFar = 1000.f;
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

HRESULT CLevel_Edit::Ready_Layer_StaticObjects(const _wstring& strLayerTag)
{
    /* 스태틱 오브젝트들 추가. */
    CProps::PROP_DESC PropDesc;
    PropDesc.iMeshIdx = 0;
    PropDesc.iShaderPassIdx = 0;

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_Props"),
        ENUM_CLASS(LEVEL::EDIT), strLayerTag, &PropDesc)))
        return E_FAIL;

    return S_OK;
}

void CLevel_Edit::Editor_GUI()
{
    ImGui::Begin("Editor");
    ImGui::BeginTabBar("Editor");

    ExportAndImport();
    Map_Editor();
    Model_Editor();
    Effect_Editor();

    ImGui::EndTabBar();
    ImGui::End();
}

void CLevel_Edit::Map_Editor()
{
    /* 맵 제어 */
    if (ImGui::BeginTabItem("Deploy"))
    {
        if (ImGui::Checkbox("Camera Activate", &m_IsCameraOn)) 
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

        ImGui::Dummy(ImVec2(0.0f, 10.f));

        ImGui::DragFloat("Mouse Speed", &m_fMouseActionSpeed, 0.02f, 0.1f, 10.0f, "%.3f");
        ImGui::Dummy(ImVec2(0.0f, 10.f));

        ImGui::Separator();

        ImGui::Text("TRANSFORM (Current Selected Object) : ");

        if (nullptr != m_pSelectedGameObject)
        {
            Show_SelectedObject();
            ImGui::Dummy(ImVec2{ 0.f,10.f });
            Edit_SelectedObject();
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

                    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_Props"), ENUM_CLASS(LEVEL::EDIT), m_strLayerMapObjectTag, &Desc)))
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

                ImGui::BeginChild(m_pGameInstance->ToString(m_strLayerMapObjectTag).c_str(), ImVec2(0, 150), true);

                size_t iObjectCnt = m_pGameInstance->Get_LayerSize(m_pGameInstance->Get_LevelID(), m_strLayerMapObjectTag);

                for (size_t i = 0; i < iObjectCnt; ++i)
                {
                    CGameObject* pGameObject = m_pGameInstance->Get_GameObject(m_pGameInstance->Get_LevelID(), m_strLayerMapObjectTag, (_uint)i);

                    _uint iMeshIdx = static_cast<CProps*>(pGameObject)->Get_MeshIdx();
                    _uint iShaderPassIdx = static_cast<CProps*>(pGameObject)->Get_ShaderPassIdx();
                    CModel* pModel = static_cast<CModel*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::EDIT), TEXT("Layer_StaticObjects"), TEXT("Com_Model"), i));
                    _string strBuffer = to_string(i) + ". Prop Info (Mesh: " + m_pGameInstance->ToString(pModel->Get_MeshName(iMeshIdx)) + ") " + "(Shader Pass: " + to_string(iShaderPassIdx) + ")";

                    if (ImGui::Selectable(strBuffer.c_str(), true)) {
                        // 선택 동작
                        m_iSelectedObjectID = (_uint)i;
                        m_pSelectedGameObject = pGameObject;
                        m_pSelectedTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(m_pGameInstance->Get_LevelID(), m_strLayerMapObjectTag, Engine::g_strTransformTag, (_uint)i));
                    }
                }

                ImGui::EndChild();

                if (ImGui::Button("Delete Object")) {
                    // 삭제 동작
                    CGameObject* pGameObject = m_pGameInstance->Get_GameObject(m_pGameInstance->Get_LevelID(), m_strLayerMapObjectTag, (_uint)m_iSelectedObjectID);
                    if (pGameObject != nullptr)
                        pGameObject->Set_Dead(true);
                }

                ImGui::EndTabItem();
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
            if (ImGui::BeginTabItem("Export Props"))
            {
                ImGui::Text("Save Props in \"Layer_StaticObjects\".");
                ImGui::Text("DONT FORGET TO SAVE PROPS BEFORE QUIT.");

                ImGui::Dummy(ImVec2(0.0f, 20.0f));
                ImGui::Text("Input Save File Path: ");

                ImGui::SameLine();
                ImGui::InputText("##Input_Path", m_szMapSavePath, IM_ARRAYSIZE(m_szMapSavePath));

                if (ImGui::Button("Export Props")) {

                    size_t iPrototypeCnt = m_ModelPrototypeTags.size();

                    for (size_t i = 0; i < iPrototypeCnt; ++i)
                    {
                        CModel* pModelCom = dynamic_cast<CModel*>(m_pGameInstance->Get_Prototype(m_pGameInstance->Get_LevelID(), m_ModelPrototypeTags[i]));
                        if (pModelCom != nullptr)
                        {
                            pModelCom->Save_Model_ToBinary(m_szModelSavePath);
                        }
                    }
                    ImGui::OpenPopup("EXPORT_PROPS_DONE");
                }

                if (ImGui::BeginPopupModal("EXPORT_PROPS_DONE", 0, ImGuiWindowFlags_NoResize))
                {
                    Align_Center("Export Props has Done");
                    ImGui::Text("Export Props has Done");

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
}
