#include "Col_KonohaVillage.h"
#include "GameInstance.h"

CCol_KonohaVillage::CCol_KonohaVillage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID )}
{
}

CCol_KonohaVillage::CCol_KonohaVillage(const CCol_KonohaVillage& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CCol_KonohaVillage::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCol_KonohaVillage::Initialize(void* pArg)
{

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_iNumMeshes = m_pModelCom->Get_NumMeshes();
    m_pGameInstance->Add_Geometry_ToPhysx(this, m_pModelCom);

    /* 영상 촬영, 테스트떄 반드시 지워야함 */
    m_pGameInstance->Add_GameObject_ToPicking(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), this, m_pModelCom);


    return S_OK;
}

void CCol_KonohaVillage::Priority_Update(_float fTimeDelta)
{
}

void CCol_KonohaVillage::Update(_float fTimeDelta)
{
}

void CCol_KonohaVillage::Late_Update(_float fTimeDelta)
{
    //m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
}

HRESULT CCol_KonohaVillage::Render()
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

HRESULT CCol_KonohaVillage::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr())))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CCol_KonohaVillage::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Model_COL_KonohaVillage"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CCol_KonohaVillage::Bind_ShaderResources()
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

CCol_KonohaVillage* CCol_KonohaVillage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CCol_KonohaVillage* pInstance = new CCol_KonohaVillage(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : COL_KonohaVillage");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCol_KonohaVillage::Clone(void* pArg)
{
    CCol_KonohaVillage* pInstance = new CCol_KonohaVillage(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : COL_KonohaVillage");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCol_KonohaVillage::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
