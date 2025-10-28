#include "Kamui.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "EffectContainer.h"
#include "EffectObject.h"
#include "ParticleObject.h"

CKamui::CKamui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CSkill {pDevice, pContext, eObjectID }
{
}

CKamui::CKamui(const CKamui& rhs)
    : CSkill { rhs }
{
}

HRESULT CKamui::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CKamui::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    KAMUI_DESC* pDesc = static_cast<KAMUI_DESC*>(pArg);

    /* Æ÷Áö¼Ç, ·è ¼¼ÆÃ. */
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
    m_pTransformCom->LookAt(m_pTransformCom->Get_State(STATE::POSITION) -1.f * XMLoadFloat3(&pDesc->vLook));
    m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) -
        3.f * m_pTransformCom->Get_State(STATE::LOOK) + XMVectorSet(0.f, 2.f, 0.f, 0.f));

    XMStoreFloat4x4(&m_ColliderWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    /* ¾Æ±â»ó¾î¶Ñ·ç·ç¶Ñ·ç */
    CEffectContainer::EFFECT_CONTAINER_DESC EffectDesc;
    EffectDesc.IsBinary = true;
    EffectDesc.strFilePath = TEXT("../Bin/Resources/Effects/Kamui_Ver1_eff.bin");
    EffectDesc.IsDistortion = true;
    EffectDesc.fSpeedRatio = 2.0f;

    m_pEffectContainer = static_cast<CEffectContainer*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EffectContainer"),
        &EffectDesc));
    m_pEffectContainer->Set_Blur(false);

    m_pGameInstance->Set_Distortion(m_fLifeTime);

    return S_OK;
}

void CKamui::Priority_Update(_float fTimeDelta)
{
    m_pEffectContainer->Priority_Update(fTimeDelta);
}

void CKamui::Update(_float fTimeDelta)
{
    m_fParticleTimeAcc += fTimeDelta;

    if (m_fParticleTimeAcc >= 0.1f && ((m_fLifeTime - m_fParticleTimeAcc) >= 0.1f))
    {
        CParticleObject::PARTICLE_LOAD_DESC ParticleDesc;
        ParticleDesc.eType = CParticleObject::PARTICLE_TYPE::EXPLOSION;
        ParticleDesc.strParticlePath = TEXT("../Bin/Resources/Particle/Kamui_Particle.bin");
        XMStoreFloat3(&ParticleDesc.vPosition, m_pTransformCom->Get_State(STATE::POSITION));

        m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"),
            m_pGameInstance->Get_LevelID(), TEXT("Layer_Particle"), &ParticleDesc);
        m_fParticleTimeAcc = 0.f;
    }

    m_fTimeAcc += fTimeDelta;
    m_fAttackCoolDown += fTimeDelta;

    if (true == m_isFinal)
    {
        m_IsDead = true;

        CParticleObject::PARTICLE_LOAD_DESC ParticleDesc;
        ParticleDesc.eType = CParticleObject::PARTICLE_TYPE::EXPLOSION;
        ParticleDesc.strParticlePath = TEXT("../Bin/Resources/Particle/Kamui_Explosion_Particle.bin");
        XMStoreFloat3(&ParticleDesc.vPosition, m_pTransformCom->Get_State(STATE::POSITION));

        m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"),
            m_pGameInstance->Get_LevelID(), TEXT("Layer_Particle"), &ParticleDesc);
    }

    if (m_fTimeAcc >= m_fLifeTime)
        m_isFinal = true;

    m_pColliderCom->Update(XMLoadFloat4x4(&m_ColliderWorldMatrix));
    m_pEffectContainer->Update(fTimeDelta);
}

void CKamui::Late_Update(_float fTimeDelta)
{
    if (false == m_isFinal && m_fAttackCoolDown >= m_fAttackMaxCoolDown)
    {
        m_fAttackCoolDown = 0.f;
        CGameManager::GetInstance()->Add_Collider_ToCollision(TEXT("Player_Skill"),
            COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_KAMUI, m_pColliderCom);
    }
    else if(true == m_isFinal)
    {
        CGameManager::GetInstance()->Add_Collider_ToCollision(TEXT("Player_Skill"),
            COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_KAMUI_END, m_pColliderCom);
    }
#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

    _float4x4 CameraWorld = *m_pGameInstance->Get_PipeLine_InverseFloat4x4(D3DTS::VIEW);

    m_pTransformCom->Set_State(STATE::RIGHT, *reinterpret_cast<_vector*>(&CameraWorld.m[0]));
    m_pTransformCom->Set_State(STATE::UP, *reinterpret_cast<_vector*>(&CameraWorld.m[1]));
    m_pTransformCom->Set_State(STATE::LOOK, *reinterpret_cast<_vector*>(&CameraWorld.m[2]));
    m_pTransformCom->Set_Scale(6.f, 6.f, 6.f);

    m_pEffectContainer->Set_ParentMatrix(XMMatrixRotationY(90.f) * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
    m_pEffectContainer->Late_Update(fTimeDelta);
    m_pEffectContainer->Add_To_Distortion();
}

HRESULT CKamui::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(12)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CKamui::Ready_Components()
{
    /* Com_VIBuffer */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    CBounding_OBB::BOUNDING_OBB_DESC		OBBDesc{};

    OBBDesc.vSize = { 10.f, 10.f, 10.f };
    OBBDesc.vCenter = { 0.f, 0.f, 0.f };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CKamui::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fKamuiLifeTime", &m_fTimeAcc, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CKamui* CKamui::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CKamui* pInstance = new CKamui(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : Kamui");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CKamui::Clone(void* pArg)
{
    CKamui* pInstance = new CKamui(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Create Failed : Kamui");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKamui::Free()
{
    __super::Free();

    Safe_Release(m_pEffectContainer);
}
