#include "LoadingPanel.h"

#include "GameInstance.h"
#include "RotationIconUI.h"

CLoadingPanel::CLoadingPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CPanel{ pDevice, pContext, eObjectID }
{
}

CLoadingPanel::CLoadingPanel(const CLoadingPanel& rhs)
    : CPanel{ rhs }
{
}

HRESULT CLoadingPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLoadingPanel::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_LoadingUIs()))
        return E_FAIL;

    return S_OK;
}

void CLoadingPanel::Priority_Update(_float fTimeDelta)
{
}

void CLoadingPanel::Update(_float fTimeDelta)
{
}

void CLoadingPanel::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CLoadingPanel::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoadingPanel::Ready_Components()
{
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingPanel"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoadingPanel::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoadingPanel::Ready_LoadingUIs()
{
    CUIObject::UIOBJECT_DESC Desc;
    Desc = CUIObject::CreateDesc(m_fX + 525, m_fY + 278, m_fZ - 0.05f, 88.f, 88.f, 0.f);
    Desc.fRotationPerSec = 1.f;

    CRotationIconUI* pRotationIcon = static_cast<CRotationIconUI*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC),
        TEXT("Prototype_GameObject_RotationIconUI"), &Desc));

    m_pGameInstance->Add_Clone_ToLayer(pRotationIcon, ENUM_CLASS(LEVEL::LOADING), TEXT("Layer_UI"));
    m_Childs.push_back(pRotationIcon);
    Safe_AddRef(pRotationIcon);

    return S_OK;
}

CLoadingPanel* CLoadingPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CLoadingPanel* pInstance = new CLoadingPanel(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed! : CLoadingPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLoadingPanel::Clone(void* pArg)
{
    CLoadingPanel* pInstance = new CLoadingPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed! : CLoadingPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLoadingPanel::Free()
{
    __super::Free();
}
