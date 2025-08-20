#include "ModelSelectButtonUI.h"

#include "GameInstance.h"

CModelSelectButtonUI::CModelSelectButtonUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CButton { pDevice, pContext, eObjectID }
{
}

CModelSelectButtonUI::CModelSelectButtonUI(const CModelSelectButtonUI& rhs)
    : CButton { rhs }
{
}

HRESULT CModelSelectButtonUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CModelSelectButtonUI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CModelSelectButtonUI::Priority_Update(_float fTimeDelta)
{
}

void CModelSelectButtonUI::Update(_float fTimeDelta)
{
}

void CModelSelectButtonUI::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CModelSelectButtonUI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CModelSelectButtonUI::Ready_Components()
{
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_Component_Texture_ModelSelectButtonUI"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CModelSelectButtonUI::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

CModelSelectButtonUI* CModelSelectButtonUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CModelSelectButtonUI* pInstance = new CModelSelectButtonUI(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : ModelSelectButton");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CModelSelectButtonUI::Clone(void* pArg)
{
    CModelSelectButtonUI* pInstance = new CModelSelectButtonUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : ModelSelectButton");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CModelSelectButtonUI::Free()
{
    __super::Free();
}
