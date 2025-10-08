#include "OutfitSelectPanel.h"

#include "GameInstance.h"

#include "CreateCharacterUI.h"

COutfitSelectPanel::COutfitSelectPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CPanel{ pDevice, pContext, eObjectID }
{
}

COutfitSelectPanel::COutfitSelectPanel(const COutfitSelectPanel& rhs)
    : CPanel { rhs }
{
}

HRESULT COutfitSelectPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT COutfitSelectPanel::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_CreateCharacterUI()))
        return E_FAIL;

    return S_OK;
}

void COutfitSelectPanel::Priority_Update(_float fTimeDelta)
{
}

void COutfitSelectPanel::Update(_float fTimeDelta)
{
}

void COutfitSelectPanel::Late_Update(_float fTimeDelta)
{
}

HRESULT COutfitSelectPanel::Render()
{
    return S_OK;
}

HRESULT COutfitSelectPanel::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

HRESULT COutfitSelectPanel::Ready_CreateCharacterUI()
{
    UIOBJECT_DESC Desc = CUIObject::CreateDesc(m_fX - g_iWinSizeX / 2.f + 275, m_fY - g_iWinSizeY / 2.f + 100, m_fZ - 0.05f, 550.f, 200.f, 0, 0.f);
    
    CCreateCharacterUI* pCreateCharacterUI = static_cast<CCreateCharacterUI*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_GameObject_CreateCharacterUI"), &Desc));
    m_pGameInstance->Add_Clone_ToLayer(pCreateCharacterUI, ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Layer_UI"));

    m_Childs.push_back(pCreateCharacterUI);
    Safe_AddRef(pCreateCharacterUI);

    return S_OK;
}

COutfitSelectPanel* COutfitSelectPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    COutfitSelectPanel* pInstance = new COutfitSelectPanel(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed! : OutfitSelect Panel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* COutfitSelectPanel::Clone(void* pArg)
{
    COutfitSelectPanel* pInstance = new COutfitSelectPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed! : OutfitSelect Panel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void COutfitSelectPanel::Free()
{
    __super::Free();
}
