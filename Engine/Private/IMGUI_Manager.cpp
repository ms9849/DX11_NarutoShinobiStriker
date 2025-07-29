#include "IMGUI_Manager.h"

#include "Prototype_Manager.h"
#include "Object_Manager.h"

CIMGUI_Manager::CIMGUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : 
     m_pDevice { pDevice }
   , m_pContext { pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CIMGUI_Manager::Initialize(HWND hWnd, class CPrototype_Manager* pPrototype_Manager, class CObject_Manager* pObject_Manager)
{
    m_pCharBuffer = new char[256];

    if (FAILED(Ready_IMGUI(hWnd)))
        return E_FAIL;

    m_pPrototype_Manager = pPrototype_Manager;
    Safe_AddRef(m_pPrototype_Manager);

    m_pObject_Manager = pObject_Manager;
    Safe_AddRef(m_pObject_Manager);

    return S_OK;
}

HRESULT CIMGUI_Manager::Ready_IMGUI(HWND hWnd)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // ImGui 스타일 고르기
    //ImGui::StyleColorsDark(); // 다크 모드
    ImGui::StyleColorsLight(); // 일반 모드

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    return S_OK;
}

void CIMGUI_Manager::Update_IMGUI()
{
    /* 여기서 Imgui 함수, 메서드, 뭐든 떄려박을 것.*/
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("IMGUI Test For Level_Edit");

    //static level 
    for (auto& iter : *m_pPrototype_Manager->m_pPrototypes)
    {
        sprintf_s(m_pCharBuffer, m_iBufferSize, "%ls", iter.first.c_str());
        ImGui::Text(m_pCharBuffer);
        ImGui::NewLine();
    }

    ImGui::End();
}

void CIMGUI_Manager::Render_IMGUI()
{
    /* 여기서 Imgui 렌더 수행해줄 것 */
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CIMGUI_Manager::Release_IMGUI()
{
    /* Free할때 실행해줄 것. */
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

CIMGUI_Manager* CIMGUI_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd,
    CPrototype_Manager* pPrototype_Manager, CObject_Manager* pObject_Manager)
{
    CIMGUI_Manager* pInstance = new CIMGUI_Manager(pDevice, pContext);
    
    if (FAILED(pInstance->Initialize(hWnd, pPrototype_Manager, pObject_Manager)))
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

    Safe_Delete_Array(m_pCharBuffer);

    Release_IMGUI();
}
