#include "FireBall.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "EffectContainer.h"
#include "EffectObject.h"
CFireBall::CFireBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CSkill { pDevice, pContext, eObjectID }
{
}

CFireBall::CFireBall(const CFireBall& rhs)
    : CSkill { rhs }
{
}

HRESULT CFireBall::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFireBall::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    FIREBALL_DESC* pDesc = static_cast<FIREBALL_DESC*>(pArg);
    
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
    m_pTransformCom->LookAt(m_pTransformCom->Get_State(STATE::POSITION) - XMLoadFloat3(&pDesc->vLook));
    m_IsEnemy = pDesc->IsEnemy;

    /* È­¿°±¸ */
    CEffectContainer::EFFECT_CONTAINER_DESC EffectDesc;
    EffectDesc.IsBinary = true;
    EffectDesc.strFilePath = TEXT("../Bin/Resources/Effects/FireBall_eff.bin");

    m_pEffectMain = static_cast<CEffectContainer*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EffectContainer"),
        &EffectDesc));
    Safe_AddRef(m_pEffectMain);
    m_pGameInstance->Add_Clone_ToLayer(m_pEffectMain, m_pGameInstance->Get_LevelID(), TEXT("Layer_Effect"));

    return S_OK;
}

void CFireBall::Priority_Update(_float fTimeDelta)
{
}

void CFireBall::Update(_float fTimeDelta)
{
    if (false == m_pColliderCom->Get_Active())
        m_IsDead = true;

    m_pTransformCom->Go_Backward(fTimeDelta, nullptr);

    m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
    m_pEffectMain->Set_ParentMatrix(XMMatrixScaling(0.5f, 0.5f, 0.5f) *  XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CFireBall::Late_Update(_float fTimeDelta)
{
    if(false == m_IsEnemy)
        CGameManager::GetInstance()->Add_Collider_ToCollision(TEXT("Player_Skill"),
            COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_FIREBALL, m_pColliderCom);
    else if(true == m_IsEnemy)
        CGameManager::GetInstance()->Add_Collider_ToCollision(TEXT("Monster_Skill"),
            COLLIDER_HANDLE_ID::ENEMY_JETSU_WOODHAND, m_pColliderCom);

    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CFireBall::Render()
{
    return S_OK;
}

HRESULT CFireBall::Ready_Components()
{
    ///* Com_Model */
    //if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototoype_Component_Model_Kamui"),
    //    TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    //    return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    CBounding_Sphere::BOUNDING_SPHERE_DESC		SphereDesc{};

    SphereDesc.fRadius = 2.f;
    SphereDesc.vCenter = { 0.f, 0.f, 0.f };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CFireBall::Bind_ShaderResources()
{
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

CFireBall* CFireBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CFireBall* pInstance = new CFireBall(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : FireBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFireBall::Clone(void* pArg)
{
    CFireBall* pInstance = new CFireBall(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : FireBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFireBall::Free()
{
    __super::Free();

    if (nullptr != m_pEffectMain)
        m_pEffectMain->Set_Dead(true);

    Safe_Release(m_pEffectMain);
}
