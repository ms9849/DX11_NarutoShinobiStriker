#include "Level_OutfitSelect.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Level_Loading.h"

#include "TestCamera.h"
#include "UIObject.h"

CLevel_OutfitSelect::CLevel_OutfitSelect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
    : CLevel { pDevice, pContext, ENUM_CLASS(eLevelID) }
    , m_pGameManager { CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

HRESULT CLevel_OutfitSelect::Initialize()
{
    if (FAILED(Ready_Lights()))
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Outfits(TEXT("Layer_Outfits"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Mannequin(TEXT("Layer_Mannequin"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
        return E_FAIL;

    if(FAILED(m_pGameManager->Change_Camera(LEVEL::OUTFITSELECT, TEXT("OutfitSelect_Camera"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_OutfitSelect::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_F3))
    {
        m_pGameInstance->Set_Visible_IMGUI(true, ENUM_CLASS(IMGUI_VISIBLE::GAMEINFO));
        m_pGameInstance->Set_Visible_IMGUI(true, ENUM_CLASS(IMGUI_VISIBLE::OBJECT_INSPECTOR));
    }

    if (m_pGameInstance->Key_Down(DIK_F4))
    {
        m_pGameInstance->Set_Visible_IMGUI(false, ENUM_CLASS(IMGUI_VISIBLE::GAMEINFO));
        m_pGameInstance->Set_Visible_IMGUI(true, ENUM_CLASS(IMGUI_VISIBLE::OBJECT_INSPECTOR));
    }

    if (m_pGameInstance->Key_Down(DIK_0))
    {
        m_pGameManager->Change_Camera(LEVEL::OUTFITSELECT, TEXT("Test_Camera"));
    }

    if (m_pGameInstance->Key_Down(DIK_9))
    {
        m_pGameManager->Change_Camera(LEVEL::OUTFITSELECT, TEXT("OutfitSelect_Camera"));
    }

    if (m_pGameInstance->Key_Down(DIK_F8))
    {
        m_pGameInstance->Request_LevelChange();
    }

    if (m_pGameInstance->IsLevelChangeRequested())
    {
        m_pGameManager->Clear();
        m_pGameInstance->Clear_Geometry();
        m_pGameInstance->Clear_Lights();
        if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::TUTORIAL))))
            return;
    }
}

HRESULT CLevel_OutfitSelect::Render()
{
    SetWindowText(g_hWnd, TEXT("외형 선택 레벨"));

    return S_OK;
}

HRESULT CLevel_OutfitSelect::Ready_Lights()
{
    /* DX11은 Dx9과 다르게 기본적인 광원 정보를 저장해주지 않는다. */
    /* 광원은 후처리 연산이 훨씬 유리하다고 하심 */
    /* Desc 세팅해서 라이트 매니저에 넣어줌. */
    LIGHT_DESC		LightDesc{};

    LightDesc.eType = LIGHT::DIRECTIONAL;
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 0.f);
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_OutfitSelect::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_GameObject_OutfitBackGround"),
        ENUM_CLASS(LEVEL::OUTFITSELECT), strLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_OutfitSelect::Ready_Layer_Camera(const _wstring& strLayerTag)
{
    /* 추후 OutfitSelect 전용 카메라로 바꿔야한다. */
    CTestCamera::TEST_CAMERA_DESC			TestCameraDesc{};

    TestCameraDesc.fFovy = XMConvertToRadians(60.0f);
    TestCameraDesc.fNear = 0.1f;
    TestCameraDesc.fFar = 500.f;
    TestCameraDesc.vEye = _float4(0.f, 1.f, -1.f, 1.f);
    TestCameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    TestCameraDesc.fSpeedPerSec = 20.f;
    TestCameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
    TestCameraDesc.fMouseSensitiy = 0.2f;

    /* 카메라는 게임 매니저에 추가하여 관리한다. */
    if (FAILED(m_pGameManager->Add_Camera(LEVEL::OUTFITSELECT, TEXT("Test_Camera"), static_cast<CCamera*>(m_pGameInstance->Clone_Prototype(
        PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_GameObject_TestCamera"), &TestCameraDesc)))))
        return E_FAIL;

    CCamera::CAMERA_DESC CameraDesc{};

    TestCameraDesc.fFovy = XMConvertToRadians(60.0f);
    TestCameraDesc.fNear = 0.1f;
    TestCameraDesc.fFar = 500.f;
    TestCameraDesc.vEye = _float4(0.f, 10.f, 10.f, 1.f);
    TestCameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    TestCameraDesc.fSpeedPerSec = 1.f;
    TestCameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(m_pGameManager->Add_Camera(LEVEL::OUTFITSELECT, TEXT("OutfitSelect_Camera"), static_cast<CCamera*>(m_pGameInstance->Clone_Prototype(
        PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_GameObject_OutfitSelectCamera"), &TestCameraDesc)))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_OutfitSelect::Ready_Layer_Outfits(const _wstring& strLayerTag)
{
    return S_OK;
}

HRESULT CLevel_OutfitSelect::Ready_Layer_Mannequin(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC Desc;
    Desc.fRotationPerSec = 180.f;
    Desc.fSpeedPerSec = 10.f;

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_GameObject_Mannequin"),
        ENUM_CLASS(LEVEL::OUTFITSELECT), strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_OutfitSelect::Ready_Layer_UI(const _wstring& strLayerTag)
{
    CUIObject::UIOBJECT_DESC Desc = CUIObject::CreateDesc(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f, 1.0f, g_iWinSizeX, g_iWinSizeY, 0, 0.f);
    
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_GameObject_OutfitSelectPanel"),
        ENUM_CLASS(LEVEL::OUTFITSELECT), strLayerTag, &Desc)))
        return E_FAIL;

    Desc = CUIObject::CreateDesc(g_iWinSizeX / 2.f + 300.f, g_iWinSizeY / 2.f + 50.f, 0.8f, g_iWinSizeX / 2.f - 200.f, g_iWinSizeY / 2.f + 100.f, 0, 0.f);

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_GameObject_ModelPanel"),
        ENUM_CLASS(LEVEL::OUTFITSELECT), strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

CLevel_OutfitSelect* CLevel_OutfitSelect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
    CLevel_OutfitSelect* pInstance = new CLevel_OutfitSelect(pDevice, pContext, eLevelID);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Create Failed: Level_OutfitSelect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_OutfitSelect::Free()
{
    __super::Free();

    Safe_Release(m_pGameManager);
}
