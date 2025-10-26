#include "Rasengan.h"

#include "GameInstance.h"

#include "GameManager.h"
#include "Player.h"

#include "EffectContainer.h"
#include "EffectObject.h"
#include "ParticleObject.h"

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

void CRasengan::Toggle_Effect()
{
    m_pEffectCharge->Set_Visible(false);
    m_pEffectRun->Set_Visible(true);
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

    /* 나선환 (차징) */
    CEffectContainer::EFFECT_CONTAINER_DESC EffectDesc;
    EffectDesc.IsBinary = true;
    EffectDesc.strFilePath = TEXT("../Bin/Resources/Effects/Rasengan_Charge_eff.bin");

    m_pEffectCharge = static_cast<CEffectContainer*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EffectContainer"),
        &EffectDesc));
    Safe_AddRef(m_pEffectCharge);
    m_pGameInstance->Add_Clone_ToLayer(m_pEffectCharge, m_pGameInstance->Get_LevelID(), TEXT("Layer_Effect"));


    /* 나선환 (달리기) */
    EffectDesc.IsBinary = true;
    EffectDesc.strFilePath = TEXT("../Bin/Resources/Effects/Rasengan_Run_eff.bin");

    m_pEffectRun = static_cast<CEffectContainer*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EffectContainer"),
        &EffectDesc));
    Safe_AddRef(m_pEffectRun);
    m_pGameInstance->Add_Clone_ToLayer(m_pEffectRun, m_pGameInstance->Get_LevelID(), TEXT("Layer_Effect"));
    m_pEffectRun->Set_Visible(false);

    CParticleObject::PARTICLE_LOAD_DESC ParticleDesc;
    ParticleDesc.eType = CParticleObject::PARTICLE_TYPE::DROP;
    //ParticleDesc.strParticlePath = TEXT("../Bin/Resources/Particle/RasenganFollow_Particle.bin");
    ParticleDesc.strParticlePath = TEXT("../Bin/Resources/Particle/RasenganFollow_Particle.bin");

    m_pParticleMain = static_cast<CParticleObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"),
        &ParticleDesc));
    Safe_AddRef(m_pParticleMain);
    m_pGameInstance->Add_Clone_ToLayer(m_pParticleMain, m_pGameInstance->Get_LevelID(), TEXT("Layer_Particle"));

    return S_OK;
}

void CRasengan::Priority_Update(_float fTimeDelta)
{
}

void CRasengan::Update(_float fTimeDelta)
{
    m_fTimeAcc += fTimeDelta;

    if (m_fTimeAcc >= 5.f || false == m_pColliderCom->Get_Active())
    {
        m_IsDead = true;
        m_pEffectRun->Set_Dead(true);
    }

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
    m_pEffectCharge->Set_ParentMatrix(XMMatrixScaling(0.25f, 0.25f, 0.25f) * XMMatrixTranslation(0.f, 0.125f, 0.f) * XMLoadFloat4x4(&m_CombinedWorldMatrix));
    m_pEffectRun->Set_ParentMatrix(XMMatrixScaling(0.25f, 0.25f, 0.25f) * XMMatrixTranslation(0.f, 0.125f, 0.f) * XMLoadFloat4x4(&m_CombinedWorldMatrix));
    m_pParticleMain->Set_Position(XMLoadFloat4(reinterpret_cast<_float4*>(&m_CombinedWorldMatrix.m[3])));
}

void CRasengan::Late_Update(_float fTimeDelta)
{
    CGameManager::GetInstance()->Add_Collider_ToCollision(TEXT("Player_Skill"), 
        COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENGAN, m_pColliderCom);

    //m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
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

    if (nullptr != m_pEffectCharge)
        m_pEffectCharge->Set_Dead(true);
    Safe_Release(m_pEffectCharge);


    if (nullptr != m_pEffectRun)
        m_pEffectRun->Set_Dead(true);
    Safe_Release(m_pEffectRun);

    if (nullptr != m_pParticleMain)
        m_pParticleMain->Set_Dead(true);
    Safe_Release(m_pParticleMain);
}
