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
    strSelectedLayer = { "Select Layer" };

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

HRESULT CIMGUI_Manager::Ready_IMGUI(HWND hWnd)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromFileTTF("../Bin/Resources/Fonts/OpenSans-SemiBold.ttf", 20.0f, NULL, io.Fonts->GetGlyphRangesKorean());
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
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.6f, 0.3f, 1.0f);
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
    memset(m_bVisibleFlag, bFlag, ENUM_CLASS(IMGUI_VISIBLE::END));
}

void CIMGUI_Manager::Release_IMGUI()
{
    /* Free할때 실행해줄 것. */
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
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

                _char pBuffer[256] = {};
                strcat_s(pBuffer, 256, to_string(iPoolCounts).c_str());
                strcat_s(pBuffer, 256, " / ");
                strcat_s(pBuffer, 256, to_string(iMaxPoolCounts).c_str());

                ImGui::ProgressBar(iPoolCounts / (_float)iMaxPoolCounts, ImVec2(0, 0), pBuffer);

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
                for (auto& pGameObjects : Layer.second->m_GameObjects)
                {
                    if (ImGui::Button(("[" + to_string(iIdx) + ']' + " Object ID: " + to_string(pGameObjects->Get_ObjectID())).c_str()))
                    {

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
    if (!m_bVisibleFlag[ENUM_CLASS(IMGUI_VISIBLE::GAMEINFO)])
        return;

    ImGui::Begin("Game Info", nullptr, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Frame: %f", 1 / fTimeDelta);
    ImGui::Separator();
    ImGui::Text("Frame Per Sec (TimeDelta): %f", fTimeDelta);
    ImGui::Separator();
    ImGui::Text("Current LevelID : %d", m_pGameInstance->Get_LevelID());

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

    Release_IMGUI();
}
