#include "Level_Edit.h"
#include "GameInstance.h"
#include "Level_Loading.h"

/* 부모의 멤버 변수 세팅은 부모에서 처리해야 한다.. */
CLevel_Edit::CLevel_Edit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID) :
    CLevel { pDevice, pContext, ENUM_CLASS(eLevelID) }
{
}

HRESULT CLevel_Edit::Initialize()
{
    /* IMGUI의 모든 GUI를 활성화 해주도록 한다. */
    m_pGameInstance->Set_Visible_All_IMGUI(true);

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Edit::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(VK_F7))
    {
        if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::GAMEPLAY))))
            return;
    }
}

HRESULT CLevel_Edit::Render()
{
    SetWindowText(g_hWnd, TEXT("EDIT LEVEL"));

    return S_OK;
}

HRESULT CLevel_Edit::Ready_Layer_Camera(const _wstring& strLayerTag)
{
    return S_OK;
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

    m_pGameInstance->Set_Visible_All_IMGUI(false);
}
