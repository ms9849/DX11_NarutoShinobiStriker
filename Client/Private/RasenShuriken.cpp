#include "RasenShuriken.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"

#include "EffectObject.h"
#include "EffectContainer.h"
#include "ParticleObject.h"

CRasenShuriken::CRasenShuriken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CSkill { pDevice, pContext, eObjectID }
{
}

CRasenShuriken::CRasenShuriken(const CRasenShuriken& rhs)
    : CSkill { rhs }
{
}

void CRasenShuriken::Throw()
{
    m_isInHand = false;
    m_isThrowing = true;

    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4((_float4*)&m_CombinedWorldMatrix.m[3]));
    m_pTransformCom->Set_Scale(4.f, 4.f, 4.f);
}

HRESULT CRasenShuriken::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRasenShuriken::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;


    RASENSHURIKEN_DESC* pDesc = static_cast<RASENSHURIKEN_DESC*>(pArg);

    m_vDirection = pDesc->vDir;
    m_pSocketMatrix = pDesc->pSocketMatrix;

    /* 수리검 던지기 */
    CEffectContainer::EFFECT_CONTAINER_DESC EffectDesc;
    EffectDesc.IsBinary = true;
    EffectDesc.strFilePath = TEXT("../Bin/Resources/Effects/RasenShuriken_eff.bin");

    m_pEffectShuriken = static_cast<CEffectContainer*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EffectContainer"),
        &EffectDesc));
    Safe_AddRef(m_pEffectShuriken);
    m_pGameInstance->Add_Clone_ToLayer(m_pEffectShuriken, m_pGameInstance->Get_LevelID(), TEXT("Layer_Effect"));

    m_pTransformCom->Set_Scale(0.1f, 0.1f, 0.1f);

    /* 폭발 */
    EffectDesc.IsBinary = true;
    EffectDesc.strFilePath = TEXT("../Bin/Resources/Effects/RasenExplosion_eff.bin");

    m_pEffectExplosion = static_cast<CEffectContainer*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EffectContainer"),
        &EffectDesc));
    Safe_AddRef(m_pEffectExplosion);
    m_pGameInstance->Add_Clone_ToLayer(m_pEffectExplosion, m_pGameInstance->Get_LevelID(), TEXT("Layer_Effect"));

    m_pTransformCom->Set_Scale(0.1f, 0.1f, 0.1f);
    m_pEffectExplosion->Set_Visible(false);

    return S_OK;
}

void CRasenShuriken::Priority_Update(_float fTimeDelta)
{
}

void CRasenShuriken::Update(_float fTimeDelta)
{
    m_fDeltaScaleTimeAcc += fTimeDelta; 

    /* 0~ 1 사이 */
    if (m_fDeltaScaleTimeAcc <= m_fDeltaScaleTime)
    {
        if (false == m_IsExplosion)
        {
            _float fScale = m_fStartScale * (m_fDeltaScaleTimeAcc / m_fDeltaScaleTime) * 35.f;
            m_pTransformCom->Set_Scale(fScale, fScale, fScale);
        }
        else
        {
            _float fScale = m_fStartScale * (m_fDeltaScaleTimeAcc / m_fDeltaScaleTime) * 80.f;
            m_pTransformCom->Set_Scale(fScale, fScale, fScale);
        }
    }

    if (true == m_isFinal)
    {
        m_IsDead = true;
        m_pEffectExplosion->Set_Dead(true);

        /* 여기서 폭발 파티클 추가 */
        CParticleObject::PARTICLE_LOAD_DESC Desc;
        Desc.strParticlePath = TEXT("../Bin/Resources/Particle/RasenShurikenExplosion_Particle.bin");
        Desc.eType = CParticleObject::PARTICLE_TYPE::EXPLOSION_NON_UV;
        XMStoreFloat3(&Desc.vPosition, m_pTransformCom->Get_State(STATE::POSITION));

        m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"), m_pGameInstance->Get_LevelID(),
            TEXT("Layer_Particle"), &Desc);
    }

    if (false == m_pColliderCom->Get_Active() && true == m_isThrowing)
    {
        m_IsHit = true;
    }

    /* 던진 상태라면 시간 쌓기 */
    if(false == m_isInHand)
        m_fTimeAcc += fTimeDelta;

    /* 던진 상태라면 시간 체크해서 사라지게 하기. */
    if (false == m_isInHand && m_fTimeAcc >= m_fLifeTime)
    {
        m_isFinal = true;
        m_pEffectShuriken->Set_Dead(true);
    }

    /* 추후 폭발로직에 맞춰 이펙트 수정해야 함. */
    if (false == m_IsHit && false == m_isInHand && true == m_isThrowing)
    {
        /* 던지는 도중에는 정해진 방향으로 날아가기. */
        /* 갑자기 사라지는 현상 체크할 것 */
        m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) +
           fTimeDelta * m_fSpeed *  XMLoadFloat3(&m_vDirection));
    }

    if (false == m_isInHand && false == m_isThrowing)
    {
        m_fAttackCoolDown += fTimeDelta;
    }

    /* 나선 수리검이 폭발하는 조건 2개 */
    if (true == m_IsHit && true == m_isThrowing)
    {
        _vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
        _vector vAdjustPosition = XMVectorSetY(vPosition, XMVectorGetY(CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform()->Get_State(STATE::POSITION)));
        m_pTransformCom->Set_State(STATE::POSITION, vAdjustPosition);
        m_pTransformCom->Set_Scale(0.1f, 0.1f, 0.1f);

        m_pEffectExplosion->Set_Visible(true);
        m_pEffectShuriken->Set_Dead(true);
        m_IsHit = false;
        m_isThrowing = false;
        m_fLifeTime = 1.5f;
        m_fTimeAcc = 0.f;
        
        m_fDeltaScaleTime = 1.f;
        m_fDeltaScaleTimeAcc = 0.f;
        m_IsExplosion = true;
    }

    if (true == m_isInHand)
    {
        _float3 vScale = m_pTransformCom->Get_Scale();
        _fmatrix ScaleMatrix = XMMatrixScaling(vScale.x, vScale.y, vScale.z);
        /* 부모 행렬 적용 */
        XMStoreFloat4x4(&m_CombinedWorldMatrix,
            ScaleMatrix * XMLoadFloat4x4(m_pSocketMatrix) *
                XMLoadFloat4x4(CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform()->Get_WorldMatrixPtr())
        );

        /* 컴바인드 매트릭스 던져주면서 자연스럽게 크기도 따라가게 됨. */
        m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
        m_pEffectShuriken->Set_ParentMatrix(XMLoadFloat4x4(&m_CombinedWorldMatrix));
        m_pEffectExplosion->Set_ParentMatrix(XMLoadFloat4x4(&m_CombinedWorldMatrix));
    }
    else
    {
        /* 현재 매트릭스 던져주기 */
        m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
        m_pEffectShuriken->Set_ParentMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
        m_pEffectExplosion->Set_ParentMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
    }

    m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 3.f);
}

void CRasenShuriken::Late_Update(_float fTimeDelta)
{
    /* 던지거나 폭발중이라면 충돌가능. */
    if (false == m_isInHand && true == m_isThrowing)
    {
        CGameManager::GetInstance()->Add_Collider_ToCollision(TEXT("Player_Skill"),
            COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENSHURIKEN, m_pColliderCom);
    }

    else if (false == m_isInHand && false == m_isThrowing && m_fAttackCoolDown >= m_fAttackMaxCoolDown)
    {
        m_fAttackCoolDown = 0.f;
        m_pColliderCom->Set_Active(true);
        CGameManager::GetInstance()->Add_Collider_ToCollision(TEXT("Player_Skill"),
            COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENSHURIKEN_EXPLODE, m_pColliderCom);
    }
    else if (false == m_isInHand && false == m_isThrowing && true == m_isFinal)
    {
        m_pColliderCom->Set_Active(true);
        CGameManager::GetInstance()->Add_Collider_ToCollision(TEXT("Player_Skill"),
            COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENSHURIKEN_END, m_pColliderCom);
    }

    //m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CRasenShuriken::Render()
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

HRESULT CRasenShuriken::Ready_Components()
{
    ///* Com_Model */
    //if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototoype_Component_Model_Rasengan"),
    //    TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    //    return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    CBounding_Sphere::BOUNDING_SPHERE_DESC		SphereDesc{};

    SphereDesc.vCenter = { 0.f, 0.f, 0.f };
    SphereDesc.fRadius = 0.5f;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRasenShuriken::Bind_ShaderResources()
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

CRasenShuriken* CRasenShuriken::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CRasenShuriken* pInstance = new CRasenShuriken(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CRasenShuriken");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRasenShuriken::Clone(void* pArg)
{
    CRasenShuriken* pInstance = new CRasenShuriken(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CRasenShuriken");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRasenShuriken::Free()
{
    __super::Free();

    Safe_Release(m_pEffectShuriken);

    if(nullptr != m_pEffectExplosion)
        Safe_Release(m_pEffectExplosion);
}
