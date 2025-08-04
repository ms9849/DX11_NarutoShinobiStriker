#include "Panel.h"

#include "TestButton.h"
#include "GameInstance.h"

CPanel::CPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CUIObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CPanel::CPanel(const CPanel& rhs)
    : CUIObject { rhs }
{
}

HRESULT CPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPanel::Initialize(void* pArg)
{
    /* Panel은 바깥에서 입력받아서 세팅해줌. */
    CUIObject::UIOBJECT_DESC* pDesc = static_cast<CUIObject::UIOBJECT_DESC*>(pArg);

    m_fX = pDesc->fX;
    m_fY = pDesc->fY;
    m_fZ = pDesc->fZ;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CPanel::Priority_Update(_float fTimeDelta)
{
}

void CPanel::Update(_float fTimeDelta)
{
}

void CPanel::Late_Update(_float fTimeDelta)
{

}

HRESULT CPanel::Render()
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

HRESULT CPanel::Ready_Components()
{
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"), 
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CPanel::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pOrthogonalCom->Bind_ViewMatrix(m_pShaderCom, "g_ViewMatrix")))
        return E_FAIL;

    if (FAILED(m_pOrthogonalCom->Bind_ProjMatrix(m_pShaderCom, "g_ProjMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    return S_OK;
}

void CPanel::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);

    for (auto& iter : m_Childs)
        Safe_Release(iter);

    m_Childs.clear();
}
