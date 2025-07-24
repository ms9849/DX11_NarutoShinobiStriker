#include "ClientPch.h"
#include "IMGUI_Manager.h"

CIMGUI_Manager::CIMGUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : 
     m_pDevice { pDevice }
   , m_pContext { pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CIMGUI_Manager::Initialize()
{
    if (FAILED(Ready_Imgui()))
        return E_FAIL;

    return S_OK;
}

HRESULT CIMGUI_Manager::Ready_Imgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // ImGui 스타일 고르기
    ImGui::StyleColorsDark(); // 다크 모드
    //ImGui::StyleColorsLight(); // 일반 모드

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    return S_OK;
}

void CIMGUI_Manager::Update_Imgui()
{
    /* 여기서 Imgui 함수, 메서드, 뭐든 떄려박을 것.*/
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("IMGUI Test For Level_Edit");

    //여기에 gui 내용을 넣으면 된다.

    ImGui::End();
}

void CIMGUI_Manager::Render_Imgui()
{
    /* 여기서 Imgui 렌더 수행해줄 것 */
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CIMGUI_Manager::Release_Imgui()
{
    /* Free할때 실행해줄 것. */
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

CIMGUI_Manager* CIMGUI_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CIMGUI_Manager* pInstance = new CIMGUI_Manager(pDevice, pContext);
    
    if (FAILED(pInstance->Initialize()))
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

    Release_Imgui();
}
