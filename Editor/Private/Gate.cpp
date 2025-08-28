#include "Gate.h"

#include "GameInstance.h"

CGate::CGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID)
    : CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CGate::CGate(const CGate& rhs)
    : CGameObject { rhs }
{
}

HRESULT CGate::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGate::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_iNumMeshes = m_pModelCom->Get_NumMeshes();

    return S_OK;
}

void CGate::Priority_Update(_float fTimeDelta)
{
}

void CGate::Update(_float fTimeDelta)
{
}

void CGate::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CGate::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    for (_uint i = 0; i < m_iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CGate::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Gate"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CGate::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CGate* CGate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID)
{
    CGate* pInstance = new CGate(pDevice, pContext, eObjectID);
    
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed ! : CGate");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGate::Clone(void* pArg)
{
    CGate* pInstance = new CGate(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed ! : CGate");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGate::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
