#include "Level_Edit.h"
#include "GameInstance.h"
#include "Level_Loading.h"

#include "TestCamera.h"

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

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Edit::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_F8))
    {
        m_pGameInstance->Set_Visible_All_IMGUI(false);

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
    CTestCamera::TEST_CAMERA_DESC			TestCameraDesc{};

    TestCameraDesc.fFovy = XMConvertToRadians(60.0f);
    TestCameraDesc.fNear = 0.1f;
    TestCameraDesc.fFar = 1000.f;
    TestCameraDesc.vEye = _float4(0.f, 30.f, -30.f, 1.f);
    TestCameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    TestCameraDesc.fSpeedPerSec = 20.f;
    TestCameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
    TestCameraDesc.fMouseSensitiy = 0.2f;

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_TestCamera"),
        ENUM_CLASS(LEVEL::EDIT), strLayerTag, &TestCameraDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Edit::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_GameObject_KonohaVillage"),
        ENUM_CLASS(LEVEL::EDIT), strLayerTag)))
        return E_FAIL;

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
}
