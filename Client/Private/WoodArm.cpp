#include "WoodArm.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "ParticleObject.h"

CWoodArm::CWoodArm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CSkill{ pDevice, pContext, eObjectID }
{
}

CWoodArm::CWoodArm(const CWoodArm& rhs)
    : CSkill{ rhs }
{
}

HRESULT CWoodArm::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWoodArm::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    WOODARM_DESC* pDesc = static_cast<WOODARM_DESC*>(pArg);
    
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&pDesc->vStartPos));
    m_pTransformCom->Set_Scale(0.03f, 0.03f, 0.03f);

    m_pTransformCom->LookAt(XMLoadFloat4(&pDesc->vLookPos));
    m_vTargetPos = pDesc->vLookPos;

    m_iNumMeshes = m_pModelCom->Get_NumMeshes();
    
    return S_OK;
}

void CWoodArm::Priority_Update(_float fTimeDelta)
{
}

void CWoodArm::Update(_float fTimeDelta)
{
    _float fDist = XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_vTargetPos) - m_pTransformCom->Get_State(STATE::POSITION)));

    //여기서 충돌소리랑 파티클
    if (fDist <= 4.f)
    {
        if (false == m_IsShaked)
        {
            /* 여기서 폭발 파티클 추가 */
            CParticleObject::PARTICLE_LOAD_DESC Desc;
            Desc.strParticlePath = TEXT("../Bin/Resources/Particle/ThousandArm_Particle.bin");
            Desc.eType = CParticleObject::PARTICLE_TYPE::EXPLOSION;
            Desc.IsBlur = false;
            XMStoreFloat3(&Desc.vPosition, m_pTransformCom->Get_State(STATE::POSITION));

            m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"), m_pGameInstance->Get_LevelID(),
                TEXT("Layer_Particle"), &Desc);

            CGameManager::GetInstance()->Shake_Camera(0.2f, 0.9f);

            m_pGameInstance->PlaySoundOnce(TEXT("WoodArm.wav"), CHANNELID::EFFECT3, 0.5f);

            m_IsShaked = true;
        }

        m_fDeadTimeAcc += fTimeDelta;

        if(m_fDeadTimeAcc >= 0.3f)
            m_pColliderCom->Set_Active(false);

        if(m_fDeadTimeAcc >= 1.f)
            m_IsDead = true;
    }
    else
        m_pTransformCom->Go_Straight(fTimeDelta, nullptr);



    m_pColliderCom->Update(XMMatrixScaling(40.f, 40.f, 40.f) * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
}

void CWoodArm::Late_Update(_float fTimeDelta)
{
    CGameManager::GetInstance()->Add_Collider_ToCollision(TEXT("Monster_Skill"),
        COLLIDER_HANDLE_ID::ENEMY_JETSU_ATTACK, m_pColliderCom);

    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CWoodArm::Render()
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

HRESULT CWoodArm::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_1000Arm_Arm"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    CBounding_OBB::BOUNDING_OBB_DESC		OBBDesc{};

    OBBDesc.vSize = { 1.1f, 1.1f, 6.f };
    OBBDesc.vCenter = { 0.f, 0.f, 3.f };
    /* Com_Collider */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CWoodArm::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CWoodArm* CWoodArm::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CWoodArm* pInstance = new CWoodArm(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CWoodArm");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWoodArm::Clone(void* pArg)
{
    CWoodArm* pInstance = new CWoodArm(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CWoodArm");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWoodArm::Free()
{
    __super::Free();
}
