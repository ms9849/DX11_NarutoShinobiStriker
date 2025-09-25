#include "Kunai.h"

#include "GameInstance.h"
#include "GameManager.h"

CKunai::CKunai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
    , m_pGameManager { CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

CKunai::CKunai(const CKunai& rhs)
    : CGameObject { rhs }
    , m_pGameManager{ CGameManager::GetInstance() }
{
        Safe_AddRef(m_pGameManager);
}

HRESULT CKunai::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CKunai::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    /* 위치 & 포지션 세팅 */
    BIRD_THROW_DESC* pDesc = static_cast<BIRD_THROW_DESC*>(pArg);
    m_vDirection = pDesc->vDirection;

    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&pDesc->vPosition));

    m_pTransformCom->LookAt(m_pTransformCom->Get_State(STATE::POSITION) + XMLoadFloat3(&m_vDirection));

    //m_iNumMeshes = m_pModelCom->Get_NumMeshes();

    return S_OK;
}

void CKunai::Priority_Update(_float fTimeDelta)
{
}

void CKunai::Update(_float fTimeDelta)
{
    if (false == m_pColliderCom->Get_Active())
        m_IsDead = true;

    m_pTransformCom->Go_Straight(fTimeDelta);

    m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CKunai::Late_Update(_float fTimeDelta)
{
    m_pGameManager->Add_Collider_ToCollision(TEXT("Monster_Attack"), COLLIDER_HANDLE_ID::ENEMY_BIRD_THROW, m_pColliderCom);

    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CKunai::Render()
{
    //if (FAILED(Bind_ShaderResources()))
    //    return E_FAIL;

    //for (_uint i = 0; i < m_iNumMeshes; ++i)
    //{
    //    if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
    //        return E_FAIL;

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

HRESULT CKunai::Ready_Components()
{
    ///* Com_Shader */
    //if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
    //    TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    //    return E_FAIL;

    ///* Com_Model */
    //if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_BirdThrow"),
    //    TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    //    return E_FAIL;

    CBounding_Sphere::BOUNDING_SPHERE_DESC ColliderDesc{};

    ColliderDesc.fRadius = 0.7f;
    ColliderDesc.vCenter = _float3{ 0.f, 0.7f, 0.f };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CKunai::Bind_ShaderResources()
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

    /* 셰이더 & Constant anem 끌고 들어가서 다 세팅? */
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

CKunai* CKunai::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CKunai* pInstance = new CKunai(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : BirdThrowObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CKunai* CKunai::Clone(void* pArg)
{
    CKunai* pInstance = new CKunai(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : BirdThrowObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKunai::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pGameManager);
}
