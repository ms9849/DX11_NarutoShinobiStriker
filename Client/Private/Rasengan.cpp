#include "Rasengan.h"

#include "GameInstance.h"

#include "GameManager.h"
#include "Player.h"

CRasengan::CRasengan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CSkill{ pDevice, pContext, eObjectID }
{
}

CRasengan::CRasengan(const CRasengan& rhs)
    : CSkill { rhs }
{
}

_bool CRasengan::IsColliderActive()
{
    return m_pColliderCom->Get_Active();
}

HRESULT CRasengan::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRasengan::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    RASENGAN_DESC* pDesc = static_cast<RASENGAN_DESC*>(pArg);
    m_pSocketMatrix = pDesc->pSocketMatrix;

    return S_OK;
}

void CRasengan::Priority_Update(_float fTimeDelta)
{
}

void CRasengan::Update(_float fTimeDelta)
{
    m_fTimeAcc += fTimeDelta;

    if (m_fTimeAcc >= 1.f)
        m_IsDead = true;

    ///* 부모 행렬 적용 */
    //XMStoreFloat4x4(&m_CombinedWorldMatrix,
    //    XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pSocketMatrix) *
    //    XMLoadFloat4x4(CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform()->Get_WorldMatrixPtr()));

    ///* 컴바인드 매트릭스 던져주면서 자연스럽게 크기도 따라가게 됨. */
    //m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));

    /* 부모 행렬 적용 */
    XMStoreFloat4x4(&m_CombinedWorldMatrix,
            XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform()->Get_WorldMatrixPtr()));

    /* 컴바인드 매트릭스 던져주면서 자연스럽게 크기도 따라가게 됨. */
    m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CRasengan::Late_Update(_float fTimeDelta)
{
    CGameManager::GetInstance()->Add_Collider_ToCollision(TEXT("Player_Skill"), 
        COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENGAN, m_pColliderCom);

    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CRasengan::Render()
{
    //if (FAILED(Bind_ShaderResources()))
    //    return E_FAIL;

    //_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    //for (size_t i = 0; i < iNumMeshes; i++)
    //{
    //    if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
    //        return E_FAIL;

    //    if (FAILED(m_pShaderCom->Begin(0)))
    //        return E_FAIL;

    //    if (FAILED(m_pModelCom->Render(i)))
    //        return E_FAIL;
    //}

#ifdef _DEBUG
    m_pColliderCom->Render();
#endif

    return S_OK;
}

HRESULT CRasengan::Ready_Components()
{
    ///* Com_Model */
    //if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototoype_Component_Model_Rasengan"),
    //    TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    //    return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    CBounding_AABB::BOUNDING_AABB_DESC		AABBDesc{};

    AABBDesc.vSize = _float3(0.7f, 0.7f, 0.7f);


    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRasengan::Bind_ShaderResources()
{
    /*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamState(STATE::POSITION), sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

CRasengan* CRasengan::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CRasengan* pInstance = new CRasengan(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : Rasengan");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRasengan::Clone(void* pArg)
{
    CRasengan* pInstance = new CRasengan(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : Rasengan");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRasengan::Free()
{
    __super::Free();
}
