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
    ImPlot::CreateContext();
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
    /* 여기서 Imgui 함수, 메서드, 뭐든 떄려박을 것.*/
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    Show_GameInfo(fTimeDelta);
    Show_ObjectInspector(fTimeDelta);
    Show_Editor(fTimeDelta);
}

void CIMGUI_Manager::Render()
{
    /* 여기서 Imgui 렌더 수행해줄 것 */
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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

void CIMGUI_Manager::Align_Center(const _char* pText)
{
    ImVec2 WindowSize = ImGui::GetWindowSize();
    _float TextSize = ImGui::CalcTextSize(pText).x + ImGui::GetStyle().FramePadding.x * 2.0f;

    ImGui::SetCursorPosX((WindowSize.x - TextSize) * 0.5f);
}

void CIMGUI_Manager::Release_IMGUI()
{
    /* Free할때 실행해줄 것. */
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();
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

void CIMGUI_Manager::Show_Editor(_float fTimeDelta)
{
    if (!m_bVisibleFlag[ENUM_CLASS(IMGUI_VISIBLE::EDITOR)])
        return;

    CTransform* pTransform = { nullptr };

    ImGui::Begin("Editor");
    ImGui::BeginTabBar("Editor");
    Map_Editor();
    Model_Editor();
    Effect_Editor();
    ImGui::EndTabBar();
    ImGui::End();
}

void CIMGUI_Manager::Map_Editor()
{
    /* 맵 제어 */
    if (ImGui::BeginTabItem("Map"))
    {
        ImGui::Text("Current Selected Object: ");
        if (ImGui::Button("Drag X", ImVec2{100.f, 30.f})) {
            // 클릭 순간 처리
        }
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            ImVec2 dragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
            ImGui::Text("Dragging: %.1f, %.1f", dragDelta.x, dragDelta.y);
        }

        ImGui::SameLine();
        ImGui::Dummy(ImVec2{ 10.f, 0.f });
        ImGui::SameLine();

        if (ImGui::Button("Drag Y", ImVec2{ 100.f, 30.f })) {
            // 클릭 순간 처리
        }
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            ImVec2 dragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
            ImGui::Text("Dragging: %.1f, %.1f", dragDelta.x, dragDelta.y);
        }

        ImGui::SameLine();
        ImGui::Dummy(ImVec2{ 10.f, 0.f });
        ImGui::SameLine();

        ImGui::Button("Wheel Z", ImVec2{ 100.f, 30.f });

        if (ImGui::IsItemHovered()) {
            _float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
                ImGui::Text("Scrolled %.1f on button", wheel);
        }

        ImGui::Separator();

        /* 드래그 / 휠 스피드 조정 */
        if (ImGui::Button("Transform Speed", ImVec2{ 400.f, 20.f })) {
            // 클릭 순간 처리
        }
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            ImVec2 dragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
            ImGui::Text("Dragging: %.1f, %.1f", dragDelta.x, dragDelta.y);
        }

        ImGui::Dummy(ImVec2(0.f, 50.f));

        if (ImGui::BeginTabBar("MAPEDIT"))
        {
            if (ImGui::BeginTabItem("Clone Prototype"))
            {
                ImGui::Text("Input Prototype Tag:   ");
                ImGui::SameLine();
                ImGui::InputText("##Input Prototype Tag", m_szClonePrototype, IM_ARRAYSIZE(m_szClonePrototype));

                ImGui::Text("Input Prototype Level: ");
                ImGui::SameLine();
                ImGui::PushItemWidth(80.f);
                ImGui::InputInt("##Input Prototype Level", &m_iClonePrototypeLevel);
                ImGui::PopItemWidth();

                ImGui::Text("Input Layer Tag:       ");
                ImGui::SameLine();
                ImGui::InputText("##Input Layer Tag", m_szLayerTag, IM_ARRAYSIZE(m_szLayerTag));

                ImGui::Text("Input Layer Level:     ");
                ImGui::SameLine();
                ImGui::PushItemWidth(80.f);
                ImGui::InputInt("##Input Layer Level", &m_iLayerLevel);
                ImGui::PopItemWidth();

                if (ImGui::Button("Clone"))
                {
                    _wstring strPrototypeTag = wstring(m_szClonePrototype, m_szClonePrototype + strlen(m_szClonePrototype));
                    _wstring strLayerTag = wstring(m_szLayerTag, m_szLayerTag + strlen(m_szLayerTag));
                    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_iClonePrototypeLevel), strPrototypeTag, m_iLayerLevel, strLayerTag)))
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

            if (ImGui::BeginTabItem("Map Objects"))
            {
                ImGui::Text("Selected Index: ");

                ImGui::BeginChild("ObjectList", ImVec2(0, 150), true);
                auto pGameObjects = m_pObject_Manager->Get_Layers(m_pGameInstance->Get_LevelID());

                for(auto& Layers : *pGameObjects)
                {
                    _string strLayerTag = m_pGameInstance->ToString(Layers.first);

                    if (strLayerTag != m_pGameInstance->ToString(g_strLayerMapObjectTag))
                        continue;

                    /* 오브젝트 인덱스 정보 찾아내야 한다. */
                    _uint iObjectIdx = { 0 };
                    auto pGameObjects = Layers.second->Get_GameObjects();

                    for (auto& pGameObject : pGameObjects)
                    {
                        _string strBuffer = to_string(iObjectIdx) + ". Test Selectable Object" + " (" + strLayerTag + ")";
                        if (ImGui::Selectable(strBuffer.c_str(), true)) {
                            // 선택 동작
                        }
                        iObjectIdx++;
                    }
                }

                ImGui::EndChild();
                
                if (ImGui::Button("Delete Object")) {
                    // 삭제 동작
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::EndTabItem();
    }
}

void CIMGUI_Manager::Model_Editor()
{
    /* 애니메이션 제어 */
    if (ImGui::BeginTabItem("Model"))
    {
        if (ImGui::BeginTabBar("Model_Control"))
        {
            if (ImGui::BeginTabItem("Converter"))
            {
                ImGui::Text("Convert FBX Model Files to Custom Binary");
                ImGui::Text("(Only Converts Prototype Models In Edit Level)");
                ImGui::Dummy(ImVec2(0.0f, 20.0f));
                ImGui::Text("Input Save File Path: ");

                ImGui::SameLine();
                ImGui::InputText("##Input_Path", m_szModelSavePath, IM_ARRAYSIZE(m_szModelSavePath));

                if (ImGui::Button("Convert")) {
                    auto pPrototypes = m_pPrototype_Manager->Get_Prototypes(m_pGameInstance->Get_LevelID());
                
                    for (auto& Prototype : *pPrototypes)
                    {
                        CModel* pModelCom = dynamic_cast<CModel*>(Prototype.second);
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

void CIMGUI_Manager::Effect_Editor()
{
    if (ImGui::BeginTabItem("Effect"))
    {

        ImGui::EndTabItem();
    }
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
