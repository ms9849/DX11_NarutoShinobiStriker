#include "Col_TutorialMap.h"
#include "GameInstance.h"

CCol_TutorialMap::CCol_TutorialMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CGameObject{ pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CCol_TutorialMap::CCol_TutorialMap(const CCol_TutorialMap& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CCol_TutorialMap::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCol_TutorialMap::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_iNumMeshes = m_pModelCom->Get_NumMeshes();
    m_pGameInstance->Add_Geometry_ToPhysx(this, m_pModelCom);

    return S_OK;
}

void CCol_TutorialMap::Priority_Update(_float fTimeDelta)
{
}

void CCol_TutorialMap::Update(_float fTimeDelta)
{
}

void CCol_TutorialMap::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CCol_TutorialMap::Render()
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

HRESULT CCol_TutorialMap::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_Component_Model_COL_TutorialMap"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CCol_TutorialMap::Bind_ShaderResources()
{
    /*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CCol_TutorialMap* CCol_TutorialMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CCol_TutorialMap* pInstance = new CCol_TutorialMap(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CCol_TutorialMap");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCol_TutorialMap::Clone(void* pArg)
{
    CCol_TutorialMap* pInstance = new CCol_TutorialMap(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CCol_TutorialMap");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCol_TutorialMap::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
