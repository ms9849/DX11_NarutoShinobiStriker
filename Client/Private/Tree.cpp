#include "Tree.h"

#include "GameInstance.h"

CTree::CTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CGameObject{ pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CTree::CTree(const CTree& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CTree::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTree::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_iNumMeshes = m_pModelCom->Get_NumMeshes();

    return S_OK;
}

void CTree::Priority_Update(_float fTimeDelta)
{
}

void CTree::Update(_float fTimeDelta)
{
}

void CTree::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CTree::Render()
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

HRESULT CTree::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Tree"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CTree::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CTree* CTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CTree* pInstance = new CTree(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed ! : CTree");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTree::Clone(void* pArg)
{
    CTree* pInstance = new CTree(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed ! : CTree");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTree::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
