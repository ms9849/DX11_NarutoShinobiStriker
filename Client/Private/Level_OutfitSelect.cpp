#include "Level_OutfitSelect.h"

CLevel_OutfitSelect::CLevel_OutfitSelect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
    : CLevel { pDevice, pContext, ENUM_CLASS(eLevelID) }
{
}

HRESULT CLevel_OutfitSelect::Initialize()
{
    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Outfits(TEXT("Layer_Outfits"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_PlayerDummy(TEXT("Layer_PlayerDummy"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_OutfitSelect::Update(_float fTimeDelta)
{
}

HRESULT CLevel_OutfitSelect::Render()
{
    return S_OK;
}

HRESULT CLevel_OutfitSelect::Ready_Layer_Camera(const _wstring& strLayerTag)
{
    return S_OK;
}

HRESULT CLevel_OutfitSelect::Ready_Layer_Outfits(const _wstring& strLayerTag)
{
    return S_OK;
}

HRESULT CLevel_OutfitSelect::Ready_Layer_PlayerDummy(const _wstring& strLayerTag)
{
    return S_OK;
}

HRESULT CLevel_OutfitSelect::Ready_Layer_UI(const _wstring& strLayerTag)
{
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
}
