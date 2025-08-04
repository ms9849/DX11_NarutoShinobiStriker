#include "IMGUI_Manager.h"

#include "Prototype_Manager.h"
#include "Pooling_Manager.h"
#include "Object_Manager.h"
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

HRESULT CIMGUI_Manager::Initialize(HWND hWnd, class CPrototype_Manager* pPrototype_Manager, class CObject_Manager* pObject_Manager, class CPooling_Manager* pPooling_Manager)
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
    io.Fonts->AddFontFromFileTTF("../Bin/Resources/Fonts/OpenSans-SemiBold.ttf", 19.0f, NULL, io.Fonts->GetGlyphRangesKorean());
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // ImGui 스타일 고르기
    //ImGui::StyleColorsDark(); // 다크 모드
    ImGui::StyleColorsLight(); // 일반 모드

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.3f, 0.6f, 0.3f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.3f, 0.6f, 0.3f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.3f, 0.6f, 0.3f, 1.0f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0, 0, 0, 0);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.4f, 0.8f, 0.4f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0, 0, 0, 0);

    return S_OK;
}

void CIMGUI_Manager::Update(_float fTimeDelta)
{
    /* 여기서 Imgui 함수, 메서드, 뭐든 떄려박을 것.*/
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    Show_Managers();
    Show_GameInfo(fTimeDelta);
    Show_ObjectInspector();
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
    memset(m_bVisibleFlag, bFlag, sizeof(_bool) * ENUM_CLASS(IMGUI_VISIBLE::END));
}

void CIMGUI_Manager::Release_IMGUI()
{
    /* Free할때 실행해줄 것. */
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();
}

void CIMGUI_Manager::Show_Managers()
{
    if (!m_bVisibleFlag[ENUM_CLASS(IMGUI_VISIBLE::MANAGER)])
        return;

    _uint iLevelID = m_pGameInstance->Get_LevelID();

    ImGui::Begin("Managers");

    Show_PoolManager(iLevelID);
    ImGui::Separator();

    Show_PrototypeManager(iLevelID);
    ImGui::Separator();


    Show_ObjectManager(iLevelID);

    ImGui::End();
}

void CIMGUI_Manager::Show_PoolManager(_uint iLevelID)
{
    if (ImGui::CollapsingHeader("Pool Manager Status"))
    {
        ImGui::Indent(20.0f);

        for (auto& iter : m_pPooling_Manager->m_PoolObjects[iLevelID])
        {
            _string strBuffer = m_pGameInstance->ToString(iter.first);

            if (ImGui::TreeNode(strBuffer.c_str()))
            {
                ImGui::Text("Pool Count");
                _uint iPoolCounts = m_pPooling_Manager->m_PoolCounts[iLevelID].find(iter.first)->second;
                size_t iMaxPoolCounts = iter.second.size();

                string strBuffer;
                strBuffer = to_string(iPoolCounts) + " / " + to_string(iMaxPoolCounts);

                ImGui::ProgressBar(iPoolCounts / (_float)iMaxPoolCounts, ImVec2(0, 0), strBuffer.c_str());

                ImGui::TreePop();
            }
        }

        ImGui::Unindent(20.0f);
    }
}

void CIMGUI_Manager::Show_PrototypeManager(_uint iLevelID)
{
    if (ImGui::CollapsingHeader("Prototype Manager Status"))
    {
        ImGui::Indent(20.0f);

        _string strBuffer;

        for (auto& iter : m_pPrototype_Manager->m_pPrototypes[iLevelID])
        {
            strBuffer = m_pGameInstance->ToString(iter.first);

            if (ImGui::Button(strBuffer.c_str()))
            {
                strSelectedPrototype = strBuffer;

                auto& iter = m_pObject_Manager->m_pLayers[iLevelID];
                strSelectedLayer = m_pGameInstance->ToString(iter.begin()->first);

                ImGui::OpenPopup("Clone Prototype");
            }
        }

        if (ImGui::BeginPopupModal("Clone Prototype"))
        {
            if (ImGui::BeginCombo("Layers", strSelectedLayer.c_str()))
            {
                for (auto& iter : m_pObject_Manager->m_pLayers[iLevelID])
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
}

void CIMGUI_Manager::Show_ObjectManager(_uint iLevelID)
{
    if (ImGui::CollapsingHeader("Object Manager Status"))
    {
        ImGui::Indent(20.0f);

        for (auto& Layer : m_pObject_Manager->m_pLayers[iLevelID])
        {
            _string strBuffer = m_pGameInstance->ToString(Layer.first);

            if (ImGui::TreeNode(strBuffer.c_str()))
            {
                ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f ), ("Object Count :" + to_string(Layer.second->m_GameObjects.size())).c_str());
                ImGui::NewLine();

                _uint iIdx = 0;
                for (auto& pGameObject : Layer.second->m_GameObjects)
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
}

void CIMGUI_Manager::Show_GameInfo(_float fTimeDelta)
{
    memmove(m_fFrames + 1, m_fFrames, sizeof(_float) * (m_iFrameArraySize - 1));
    m_fFrames[0] = 1 / fTimeDelta;
 
    if (!m_bVisibleFlag[ENUM_CLASS(IMGUI_VISIBLE::GAMEINFO)])
        return;

    ImGui::Begin("Game Info", nullptr, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Current LevelID : %d", m_pGameInstance->Get_LevelID());
    ImGui::Separator();

    ImGui::Text("Frame: %f", 1 / fTimeDelta);
    ImGui::Separator();

    ImGui::Text("Frame Per Sec (TimeDelta): %f", fTimeDelta);
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Frame Visualization"))
    {
        if (ImPlot::BeginPlot("##spark", ImVec2(-1, -1), ImPlotFlags_CanvasOnly)) {
            //ImPlot::SetupAxisFormat(ImAxis_Y1, "%.6f");
            ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations,  ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks);
            ImPlot::SetupAxesLimits(0, 99, m_fFrameRange[0], m_fFrameRange[1], ImGuiCond_Always);
            ImPlot::SetupAxisTicks(ImAxis_Y1, m_fFrameRange, 2);

            ImPlot::SetNextLineStyle(ImVec4(0.f, 0.f, 0.f, 1.f));
            ImPlot::PlotLine("##spark", m_fFrames, 100, 3.f);

            ImPlot::EndPlot();
        }
    }

    ImGui::End();
}

void CIMGUI_Manager::Show_ObjectInspector()
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
                            pTransform->Rotation(pTransform->Get_State(STATE::RIGHT), m_fAngleRight);
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
                            pTransform->Rotation(pTransform->Get_State(STATE::UP), m_fAngleUp);
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
                            pTransform->Rotation(pTransform->Get_State(STATE::LOOK), m_fAngleLook);
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
    ImGui::End();
}

CIMGUI_Manager* CIMGUI_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd,
    CPrototype_Manager* pPrototype_Manager, CObject_Manager* pObject_Manager, CPooling_Manager* pPooling_Manager)
{
    CIMGUI_Manager* pInstance = new CIMGUI_Manager(pDevice, pContext);
    
    if (FAILED(pInstance->Initialize(hWnd, pPrototype_Manager, pObject_Manager, pPooling_Manager)))
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
    Safe_Release(m_pGameInstance);

    Safe_Release(m_pSelectedGameObject);
    Safe_Delete_Array(m_fFrames);

    Release_IMGUI();
}
