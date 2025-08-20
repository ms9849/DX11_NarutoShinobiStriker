#include "ModelPanel.h"

#include "GameInstance.h"

CModelPanel::CModelPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CPanel { pDevice, pContext, eObjectID }
{
}

CModelPanel::CModelPanel(const CModelPanel& rhs)
    : CPanel{ rhs }
{
}

HRESULT CModelPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CModelPanel::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Buttons()))
        return E_FAIL;

    if (FAILED(Ready_DecideButton()))
        return E_FAIL;

    return S_OK;
}

void CModelPanel::Priority_Update(_float fTimeDelta)
{
}

void CModelPanel::Update(_float fTimeDelta)
{
}

void CModelPanel::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CModelPanel::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CModelPanel::Ready_Components()
{
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_Component_Texture_ModelPanel"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CModelPanel::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

HRESULT CModelPanel::Ready_Buttons()
{
    return S_OK;
}

HRESULT CModelPanel::Ready_DecideButton()
{
    return S_OK;
}

CModelPanel* CModelPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CModelPanel* pInstance = new CModelPanel(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CModelPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CModelPanel::Clone(void* pArg)
{
    CModelPanel* pInstance = new CModelPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CModelPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CModelPanel::Free()
{
    __super::Free();
}
