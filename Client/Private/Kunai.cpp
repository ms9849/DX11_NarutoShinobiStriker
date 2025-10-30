#include "Kunai.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Trail.h"

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

    /* 위치 & 포지션 세팅 */
    KUNAI_DESC* pDesc = static_cast<KUNAI_DESC*>(pArg);
    m_vDirection = pDesc->vDirection;

    m_pTransformCom->Set_Scale(1.2f, 1.2f, 1.2f);
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.5f, 0.f, 0.f) + XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
    m_pTransformCom->LookAt(m_pTransformCom->Get_State(STATE::POSITION) + XMLoadFloat3(&m_vDirection));
    m_eType = pDesc->eType;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_iNumMeshes = m_pModelCom->Get_NumMeshes();


    CTrail::TRAIL_DESC TrailDesc;
    TrailDesc.IsBlend = true;
    TrailDesc.vFootTrailColor = _float4(1.0f, 0.3f, 0.3f, 1.f);
    TrailDesc.strTrailTextureTag = TEXT("Prototype_Component_Texture_FootTrail_Blue");
    XMStoreFloat4(&TrailDesc.vHighPosition, XMVectorSet(-0.02f, 0.f, 0.f, 1.f));
    XMStoreFloat4(&TrailDesc.vLowPosition, XMVectorSet(0.f, 0.f, 0.02f, 1.f));

    m_pTrail = static_cast<CTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_KunaiTrail"), &TrailDesc));

    return S_OK;
}

void CKunai::Priority_Update(_float fTimeDelta)
{
}

void CKunai::Update(_float fTimeDelta)
{
    if (false == m_pColliderCom->Get_Active())
    {
        m_fTimeAcc += fTimeDelta;

        if(1.0 <= m_fTimeAcc)
            m_IsDead = true;
    }

    m_pTransformCom->Go_Straight(fTimeDelta);

    m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    m_fTrailTimeAcc += fTimeDelta;

    if (m_fTrailTimeAcc >= m_fTrailTime)
    {
        m_pTrail->Update_Trail(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()), true);
        m_fTrailTimeAcc = 0.f;
    }
}

void CKunai::Late_Update(_float fTimeDelta)
{
    m_pGameManager->Add_Collider_ToCollision(TEXT("Monster_Attack"), COLLIDER_HANDLE_ID::ENEMY_THROW, m_pColliderCom);

    m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, this);

    m_pTrail->Late_Update(fTimeDelta);
#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CKunai::Render()
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

HRESULT CKunai::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (m_eType == KUNAI_TYPE::KUNAI)
    {
        if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Kunai"),
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
    }
    else
    {
        if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Kunai_Diff"),
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
    }

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

    Safe_Release(m_pTrail);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pGameManager);
}
