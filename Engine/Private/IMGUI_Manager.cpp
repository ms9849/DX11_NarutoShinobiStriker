#include "IMGUI_Manager.h"

#include "Prototype_Manager.h"
#include "Pooling_Manager.h"
#include "Object_Manager.h"
#include "Picking_Manager.h"
#include "GameObject.h"
#include "Layer.h"

#include "GameInstance.h"

CIMGUI_Manager::CIMGUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) 
    : m_pDevice { pDevice }
    , m_pContext { pContext }
    , m_pGameInstance { CGameInstance::GetInstance() }
    
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}


HRESULT CIMGUI_Manager::Initialize(HWND hWnd, CPrototype_Manager* pPrototype_Manager, CObject_Manager* pObject_Manager, CPooling_Manager* pPooling_Manager, CPicking_Manager* pPicking)
{
    memset(m_bVisibleFlag, 0, ENUM_CLASS(IMGUI_VISIBLE::END));

    m_iFrameArraySize = 100;
    m_fFrames = new _float[m_iFrameArraySize];
    memset(m_fFrames, 0, m_iFrameArraySize);

    m_vObjectScale = _float3{ 1.f, 1.f, 1.f };

    if (FAILED(Ready_IMGUI(hWnd)))
        return E_FAIL;

    m_pPrototype_Manager = pPrototype_Manager;
    Safe_AddRef(m_pPrototype_Manager);

    m_pObject_Manager = pObject_Manager;
    Safe_AddRef(m_pObject_Manager);

    m_pPooling_Manager = pPooling_Manager;
    Safe_AddRef(m_pPooling_Manager);

    m_pPicking_Manager = pPicking;
    Safe_AddRef(m_pPicking_Manager);

    return S_OK;
}

void CIMGUI_Manager::Clear()
{
    if (nullptr != m_pSelectedGameObject)
    {
        Safe_Release(m_pSelectedGameObject);
        m_pSelectedGameObject = nullptr;
    }

    m_strSelectedLayer = {};
    m_iSelectedObjectID = {};
    m_iSelectedObjectIndex = {};

    m_vObjectScale = _float3{ 1.f, 1.f, 1.f };
    m_vObjectPos = {};
    m_fAngleRight = { 0.f }, m_fAngleUp = { 0.f }, m_fAngleLook = { 0.f };
}

HRESULT CIMGUI_Manager::Ready_IMGUI(HWND hWnd)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // ImGui 스타일 고르기
    ImGui::StyleColorsDark(); // 다크 모드
    //ImGui::StyleColorsLight(); // 일반 모드

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.f, 0.7f, 0.f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0, 0, 0, 0);

    return S_OK;
}

void CIMGUI_Manager::Update(_float fTimeDelta)
{
    /* 여기서 엔진단에서 제공해야될 정보들을 IMGUI로 작성할 것.*/
    Show_GameInfo(fTimeDelta);
    Show_ObjectInspector(fTimeDelta);
}

void CIMGUI_Manager::Set_Visible_IMGUI(_bool bFlag, _uint iIMGUIID)
{
    m_bVisibleFlag[iIMGUIID] = bFlag;
}

void CIMGUI_Manager::Set_Visible_All_IMGUI(_bool bFlag)
{
    for (_uint i = 0; i < ENUM_CLASS(IMGUI_VISIBLE::END); ++i)
    {
        m_bVisibleFlag[i] = bFlag;
    }
}

void CIMGUI_Manager::Release_IMGUI()
{
    /* Free할때 실행해줄 것. */
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void CIMGUI_Manager::Show_GameInfo(_float fTimeDelta)
{
    memmove(m_fFrames + 1, m_fFrames, sizeof(_float) * (m_iFrameArraySize - 1));
    m_fFrames[0] = 1 / fTimeDelta;
 
    if (!m_bVisibleFlag[ENUM_CLASS(IMGUI_VISIBLE::GAMEINFO)])
        return;

    _uint iLevelID = m_pGameInstance->Get_LevelID();

    ////탭바 
    ImGui::Begin("InGame Info", &m_bVisibleFlag[ENUM_CLASS(IMGUI_VISIBLE::GAMEINFO)], ImGuiWindowFlags_NoCollapse);

    if (ImGui::BeginTabBar("TEST"))
    {
        if (ImGui::BeginTabItem("Info"))
        {
            ImGui::Text("Current LevelID : %d", iLevelID);
            ImGui::Separator();

            ImGui::Text("Frame: %f", 1 / fTimeDelta);
            ImGui::Separator();

            ImGui::Text("Frame Per Sec (TimeDelta): %f", fTimeDelta);
            ImGui::Separator();

            CTransform* pTransform = { nullptr };
            _string strBuffer;

            strBuffer = "World Ray Pos: ";
            ImGui::Text(strBuffer.c_str());

            ImGui::PushItemWidth(70);

            _float3 vRayPos = m_pPicking_Manager->Get_RayDir(RAY::WORLD);

            ImGui::InputFloat("##Xpos", &vRayPos.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
            ImGui::SameLine();
            ImGui::InputFloat("##Ypos", &vRayPos.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
            ImGui::SameLine();

            ImGui::InputFloat("##Zpos", &vRayPos.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
            ImGui::PopItemWidth();


            strBuffer = "World Ray Dir: ";
            ImGui::Text(strBuffer.c_str());
            ImGui::PushItemWidth(70);

            _float3 vRayDir = m_pPicking_Manager->Get_RayDir(RAY::WORLD);

            ImGui::InputFloat("##Xdir ", &vRayDir.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
            ImGui::SameLine();
            ImGui::InputFloat("##Ydir", &vRayDir.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
            ImGui::SameLine();
            ImGui::InputFloat("##Zdir", &vRayDir.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);

            ImGui::PopItemWidth();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Managers"))
        {
            /* 풀 매니저 인포 */
            if (ImGui::CollapsingHeader("Pool Manager"))
            {
                ImGui::Indent(20.0f);

                auto PoolObjects = *m_pPooling_Manager->Get_PoolObjects(iLevelID);
                for (auto& iter : PoolObjects)
                {
                    _string strBuffer = m_pGameInstance->ToString(iter.first);

                    if (ImGui::TreeNode(strBuffer.c_str()))
                    {
                        ImGui::Text("Pool Count");

                        _uint iPoolCounts = (m_pPooling_Manager->Get_PoolCounts(iLevelID))->find(iter.first)->second;
                        size_t iMaxPoolCounts = iter.second.size();

                        string strBuffer;
                        strBuffer = to_string(iPoolCounts) + " / " + to_string(iMaxPoolCounts);

                        ImGui::ProgressBar(iPoolCounts / (_float)iMaxPoolCounts, ImVec2(0, 0), strBuffer.c_str());

                        ImGui::TreePop();
                    }
                }

                ImGui::Unindent(20.0f);
            }
            /* 프로토타입 매니저 인포 */
            if (ImGui::CollapsingHeader("Prototype Manager"))
            {
                ImGui::Indent(20.0f);

                _string strBuffer;

                auto Prototypes = m_pPrototype_Manager->Get_Prototypes(iLevelID);
                for (auto& iter : *Prototypes)
                {
                    strBuffer = m_pGameInstance->ToString(iter.first);

                    if (ImGui::Button(strBuffer.c_str()))
                    {
                        strSelectedPrototype = strBuffer;

                        auto pLayers = m_pObject_Manager->Get_Layers(iLevelID);
                        strSelectedLayer = m_pGameInstance->ToString(pLayers->begin()->first);

                        ImGui::OpenPopup("Clone Prototype");
                    }
                }

                if (ImGui::BeginPopupModal("Clone Prototype"))
                {
                    if (ImGui::BeginCombo("Layers", strSelectedLayer.c_str()))
                    {
                        auto pLayers = m_pObject_Manager->Get_Layers(iLevelID);
                        for (auto& iter : *pLayers)
                        {
                            _string strBufferSecond = m_pGameInstance->ToString(iter.first);

                            if (ImGui::Selectable(strBufferSecond.c_str(), false))
                            {
                                strSelectedLayer = strBufferSecond;
                            }
                        }

                        ImGui::EndCombo();
                    }

                    ImGui::Separator();

                    ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Prototype: ");
                    ImGui::SameLine();
                    ImGui::Text(strSelectedPrototype.c_str());

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.3f, 1.0f));

                    if (ImGui::Button("Clone"))
                    {
                        //지금은 단순히 같은 레벨에 클론해주지만.. 개선이 가능함
                        m_pGameInstance->Add_GameObject_ToLayer(iLevelID, m_pGameInstance->ToWstring(strSelectedPrototype), iLevelID, m_pGameInstance->ToWstring(strSelectedLayer), nullptr);
                    }
                    ImGui::PopStyleColor();

                    ImGui::SameLine();

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.f, 0.f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.f, 0.f, 1.0f));

                    if (ImGui::Button("Close"))
                        ImGui::CloseCurrentPopup();

                    ImGui::PopStyleColor();
                    ImGui::PopStyleColor();

                    ImGui::EndPopup();
                }

                ImGui::Unindent(20.0f);
            }
            /* 오브젝트 매니저 인포 */
            if (ImGui::CollapsingHeader("Object Manager"))
            {
                ImGui::Indent(20.0f);

                auto pLayers = m_pObject_Manager->Get_Layers(iLevelID);
                for (auto& Layer : *pLayers)
                {
                    _string strBuffer = m_pGameInstance->ToString(Layer.first);

                    if (ImGui::TreeNode(strBuffer.c_str()))
                    {
                        auto GameObjects = Layer.second->Get_GameObjects();

                        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), ("Object Count :" + to_string(GameObjects.size())).c_str());
                        ImGui::NewLine();

                        _uint iIdx = 0;

                        for (auto pGameObject : GameObjects)
                        {
                            if (ImGui::Button(("[" + to_string(iIdx) + ']' + " Object ID: " + to_string(pGameObject->Get_ObjectID())).c_str()))
                            {
                                if (nullptr != m_pSelectedGameObject)
                                    Safe_Release(m_pSelectedGameObject);

                                m_pSelectedGameObject = pGameObject;
                                Safe_AddRef(m_pSelectedGameObject);
                                m_strSelectedLayer = strBuffer;
                                m_iSelectedObjectID = pGameObject->Get_ObjectID();
                                m_iSelectedObjectIndex = iIdx;
                            }
                            iIdx++;
                        }

                        ImGui::TreePop();
                    }

                    ImGui::Separator();
                }

                ImGui::Unindent(20.0f);
            }

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

void CIMGUI_Manager::Show_ObjectInspector(_float fTimeDelta)
{
    if (!m_bVisibleFlag[ENUM_CLASS(IMGUI_VISIBLE::OBJECT_INSPECTOR)])
        return;

    if (m_pSelectedGameObject == nullptr)
        return;

    CTransform* pTransform = { nullptr };
    _float4 vSource;
    _string strBuffer;
    
    pTransform = static_cast<CTransform*>(m_pSelectedGameObject->Find_Component(g_strTransformTag));

    ImGui::Begin("Object Inspector");
        ImGui::BeginTabBar("Info"); 
            if (ImGui::BeginTabItem("Info"))
            {
                if (m_pSelectedGameObject != nullptr)
                {
                    strBuffer = "Layer: " + m_strSelectedLayer;
                    ImGui::Text(strBuffer.c_str());

                    strBuffer = "Object ID: " + to_string(m_iSelectedObjectID);
                    ImGui::Text(strBuffer.c_str());

                    strBuffer = "Object Index: " + to_string(m_iSelectedObjectIndex);
                    ImGui::Text(strBuffer.c_str());
                }
                ImGui::EndTabItem();
            }
        ImGui::EndTabBar();

        ImGui::BeginTabBar("Components");
            if (ImGui::BeginTabItem("Transform"))
            {
                if (m_pSelectedGameObject != nullptr)
                {
                    ImGui::PushItemWidth(70);

                    ImGui::Text("RIGHT ");
                    ImGui::SameLine();
                    XMStoreFloat4(&vSource, pTransform->Get_State(STATE::RIGHT));
                    ImGui::InputFloat("##1,1", &vSource.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    ImGui::SameLine();
                    ImGui::InputFloat("##1,2", &vSource.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    ImGui::SameLine();
                    ImGui::InputFloat("##1,3", &vSource.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    ImGui::SameLine();
                    ImGui::InputFloat("##1,4", &vSource.w, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);

                    ImGui::Text("UP \t  ");
                    ImGui::SameLine();
                    XMStoreFloat4(&vSource, pTransform->Get_State(STATE::UP));
                    ImGui::InputFloat("##2,1", &vSource.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    ImGui::SameLine();
                    ImGui::InputFloat("##2,2", &vSource.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    ImGui::SameLine();
                    ImGui::InputFloat("##2,3", &vSource.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    ImGui::SameLine();
                    ImGui::InputFloat("##2,4", &vSource.w, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);

                    ImGui::Text("LOOK  ");
                    ImGui::SameLine();
                    XMStoreFloat4(&vSource, pTransform->Get_State(STATE::LOOK));
                    ImGui::InputFloat("##3,1", &vSource.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    ImGui::SameLine();
                    ImGui::InputFloat("##3,2", &vSource.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    ImGui::SameLine();
                    ImGui::InputFloat("##3,3", &vSource.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    ImGui::SameLine();
                    ImGui::InputFloat("##3,4", &vSource.w, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);

                    ImGui::Text("POS \t");
                    ImGui::SameLine();
                    XMStoreFloat4(&vSource, pTransform->Get_State(STATE::POSITION));
                    ImGui::InputFloat("##4,1", &vSource.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    ImGui::SameLine();
                    ImGui::InputFloat("##4,2", &vSource.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    ImGui::SameLine();
                    ImGui::InputFloat("##4,3", &vSource.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    ImGui::SameLine();
                    ImGui::InputFloat("##4,4", &vSource.w, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);

                    ImGui::PopItemWidth();

                    ImGui::BeginTabBar("Transform Inspector");
                    if (ImGui::BeginTabItem("Scale"))
                    {
                        ImGui::PushItemWidth(70);

                        ImGui::Text("Input Scale");
                        ImGui::InputFloat("##ScaleX", &m_vObjectScale.x, 0.0f, 0.0f, "%.3f");
                        ImGui::SameLine();
                        ImGui::InputFloat("##ScaleY", &m_vObjectScale.y, 0.0f, 0.0f, "%.3f");
                        ImGui::SameLine();
                        ImGui::InputFloat("##ScaleZ", &m_vObjectScale.z, 0.0f, 0.0f, "%.3f");
                        ImGui::SameLine();

                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.3f, 1.0f));
                        if (ImGui::Button("Apply"))
                        {
                            pTransform->Set_Scale(m_vObjectScale.x, m_vObjectScale.y, m_vObjectScale.z);
                            m_vObjectScale = _float3{ 1.f, 1.f, 1.f };
                        }
                        ImGui::PopStyleColor();

                        ImGui::PopItemWidth();
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Rotation"))
                    {
                        ImGui::PushItemWidth(70);

                        ImGui::Text("Rotation By Axis Right ");
                        ImGui::SameLine();
                        ImGui::InputFloat("##AngleRight", &m_fAngleRight, 0.0f, 0.0f, "%.3f");
                        ImGui::SameLine();
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.3f, 1.0f));

                        ImGui::PushID("Apply Right");
                        if (ImGui::Button("Apply"))
                        {
                            pTransform->Rotation(pTransform->Get_State(STATE::RIGHT), XMConvertToRadians(m_fAngleRight));
                            m_fAngleRight = 0.f;
                        }
                        ImGui::PopID();
                        ImGui::PopStyleColor();

                        ImGui::Separator();

                        ImGui::Text("Rotation By Axis Up\t ");
                        ImGui::SameLine();
                        ImGui::InputFloat("##AngleUp", &m_fAngleUp, 0.0f, 0.0f, "%.3f");
                        ImGui::SameLine();
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.3f, 1.0f));

                        ImGui::PushID("Apply Up");
                        if (ImGui::Button("Apply"))
                        {
                            pTransform->Rotation(pTransform->Get_State(STATE::UP), XMConvertToRadians(m_fAngleUp));
                            m_fAngleUp = 0.f;
                        }
                        ImGui::PopID();
                        ImGui::PopStyleColor();

                        ImGui::Separator();

                        ImGui::Text("Rotation By Axis Look ");
                        ImGui::SameLine();
                        ImGui::InputFloat("##AngleLook", &m_fAngleLook, 0.0f, 0.0f, "%.3f");
                        ImGui::SameLine();
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.3f, 1.0f));

                        ImGui::PushID("Apply Look");
                        if (ImGui::Button("Apply"))
                        {
                            pTransform->Rotation(pTransform->Get_State(STATE::LOOK), XMConvertToRadians(m_fAngleLook));
                            m_fAngleLook = 0.f;
                        }
                        ImGui::PopID();
                        ImGui::PopStyleColor();

                        ImGui::PopItemWidth();
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Position"))
                    {
                        ImGui::PushItemWidth(70);
                        ImGui::Text("Input Position");
                        ImGui::InputFloat("##PosX", &m_vObjectPos.x, 0.0f, 0.0f, "%.3f");
                        ImGui::SameLine();
                        ImGui::InputFloat("##PosY", &m_vObjectPos.y, 0.0f, 0.0f, "%.3f");
                        ImGui::SameLine();
                        ImGui::InputFloat("##PosZ", &m_vObjectPos.z, 0.0f, 0.0f, "%.3f");
                        ImGui::SameLine();

                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.3f, 1.0f));
                        if (ImGui::Button("Apply"))
                        {
                            pTransform->Set_State(STATE::POSITION, XMVectorSet(m_vObjectPos.x, m_vObjectPos.y, m_vObjectPos.z, 1.f));
                            m_vObjectPos = { 0.f, 0.f, 0.f };
                        }
                        ImGui::PopStyleColor();

                        ImGui::PopItemWidth();
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Test Tab Bar"))
            {
                ImGui::Text("Test For Tab Bar");
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();

            ImGui::Separator();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.f, 0.f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.f, 0.f, 1.0f));

            if (ImGui::Button("Close"))
            {
                Safe_Release(m_pSelectedGameObject);
                m_pSelectedGameObject = nullptr;

            }

            ImGui::PopStyleColor();
            ImGui::PopStyleColor();

            ImGui::End();
}

CIMGUI_Manager* CIMGUI_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd,
    CPrototype_Manager* pPrototype_Manager, CObject_Manager* pObject_Manager, CPooling_Manager* pPooling_Manager, CPicking_Manager* pPicking)
{
    CIMGUI_Manager* pInstance = new CIMGUI_Manager(pDevice, pContext);
    
    if (FAILED(pInstance->Initialize(hWnd, pPrototype_Manager, pObject_Manager, pPooling_Manager, pPicking)))
    {
        MSG_BOX("Create Failed : IMGUI_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CIMGUI_Manager::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    Safe_Release(m_pPrototype_Manager);
    Safe_Release(m_pObject_Manager);
    Safe_Release(m_pPooling_Manager);
    Safe_Release(m_pPicking_Manager);

    Safe_Release(m_pGameInstance);

    Safe_Release(m_pSelectedGameObject);
    Safe_Delete_Array(m_fFrames);

    Release_IMGUI();
}
