#include "WhiteJetsu.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "WhiteJetsu_IdleState.h"
#include "WhiteJetsu_BeatenState.h"
#include "WhiteJetsu_BeatenBlastedState.h"
#include "WhiteJetsu_ElectricShockState.h"
#include "WhiteJetsu_DeadState.h"

#include "ParticleObject.h"

#include "Player.h"

CWhiteJetsu::CWhiteJetsu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CEnemy { pDevice, pContext, eObjectID }
{
}

CWhiteJetsu::CWhiteJetsu(const CWhiteJetsu& rhs)
    : CEnemy{ rhs }
{
}

_wstring CWhiteJetsu::Get_CurrentAnim()
{
    return m_pModelCom->Get_CurrentAnim();
}

_float CWhiteJetsu::Get_AnimProgress()
{
    return m_pModelCom->Get_CurAnimProgress();
}

void CWhiteJetsu::Set_AnimProgress(_float fProgress)
{
	m_pModelCom->Set_CurAnimProgress(fProgress);
}

void CWhiteJetsu::Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate, _bool IsBlend, _float fBlendRatio, _bool IsLoop)
{
	m_pModelCom->Set_AnimIndex(pAnimName, fAnimationPlayRate, IsBlend, fBlendRatio, IsLoop);
}

_bool CWhiteJetsu::Play_Animation(_float fTimeDelta)
{
	return m_pModelCom->Play_Animation(fTimeDelta);
}

void CWhiteJetsu::Set_Collider_Active(const _wstring& strColliderTag, _bool bFlag)
{
    static_cast<CCollider*>(Find_Component(strColliderTag))->Set_Active(bFlag);
}

CCollider* CWhiteJetsu::Get_Collider(const _wstring& strColliderTag)
{
    return static_cast<CCollider*>(Find_Component(strColliderTag));
}

void CWhiteJetsu::Calc_HitEffectTime(_float fTimeDelta)
{
    m_fEffectTimeAcc += fTimeDelta;

    if (m_fEffectTimeAcc >= m_fEffectTime)
    {
        m_fEffectTimeAcc = 0.f;
        m_iShaderPassIdx = 0;
        m_fEffectTime = 0.f;
    }
}

void CWhiteJetsu::Set_HitEffect(_float fEffectTime, _float fIntensity)
{
    m_iShaderPassIdx = 2;
    m_fIntensity = fIntensity;
    m_fEffectTime = fEffectTime;
}

void CWhiteJetsu::OnCollision(COLLIDER_HANDLE_ID eHandleID)
{
    if (true == m_IsInvincible || true == m_IsPlayingDeadAnim)
        return;
    
    m_pGameManager->Active_Combo();

    CWhiteJetsuState* pNextState = { nullptr };

    _vector vDirection = m_pTransformCom->Get_State(STATE::POSITION) -
        CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform()->Get_State(STATE::POSITION);

    _float fHitTime = { 0.3f };

    if (COLLIDER_HANDLE_ID::PLAYER_HAND_ATTACK == eHandleID)
    {
        CParticleObject::PARTICLE_LOAD_DESC Desc;
        Desc.strParticlePath = TEXT("../Bin/Resources/Particle/Player_Attack_Particle.bin");
        XMStoreFloat3(&Desc.vPosition, m_pTransformCom->Get_State(STATE::POSITION));

        m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"), m_pGameInstance->Get_LevelID(),
            TEXT("Layer_Particle"), &Desc);

        m_fCurrentHP -= 1.f;
        pNextState = CWhiteJetsu_BeatenState::Create(m_pNavigationCom, this, vDirection, 1.8f);
    }
    else if (COLLIDER_HANDLE_ID::PLAYER_HAND_ATTACK_FINAL == eHandleID)
    {
        CParticleObject::PARTICLE_LOAD_DESC Desc;
        Desc.strParticlePath = TEXT("../Bin/Resources/Particle/Player_Attack_Particle.bin");
        XMStoreFloat3(&Desc.vPosition, m_pTransformCom->Get_State(STATE::POSITION));

        m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"), m_pGameInstance->Get_LevelID(),
            TEXT("Layer_Particle"), &Desc);

        m_fCurrentHP -= 3.f;
        pNextState = CWhiteJetsu_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_SWORD_ATTACK== eHandleID)
    {
        CParticleObject::PARTICLE_LOAD_DESC Desc;
        Desc.strParticlePath = TEXT("../Bin/Resources/Particle/Player_Attack_Particle.bin");
        XMStoreFloat3(&Desc.vPosition, m_pTransformCom->Get_State(STATE::POSITION));

        m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"), m_pGameInstance->Get_LevelID(),
            TEXT("Layer_Particle"), &Desc);

        m_fCurrentHP -= 3.f;

        pNextState = CWhiteJetsu_BeatenState::Create(m_pNavigationCom, this, vDirection, 1.75f);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_SWORD_ATTACK_FINAL == eHandleID)
    {
        CParticleObject::PARTICLE_LOAD_DESC Desc;
        Desc.strParticlePath = TEXT("../Bin/Resources/Particle/Player_Attack_Particle.bin");
        XMStoreFloat3(&Desc.vPosition, m_pTransformCom->Get_State(STATE::POSITION));

        m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"), m_pGameInstance->Get_LevelID(),
            TEXT("Layer_Particle"), &Desc);

        m_fCurrentHP -= 3.f;

        pNextState = CWhiteJetsu_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENGAN == eHandleID ||
        COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_FIREBALL == eHandleID)
    {
        m_fCurrentHP -= 15.f;

        pNextState = CWhiteJetsu_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENSHURIKEN == eHandleID ||
        COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENSHURIKEN_EXPLODE == eHandleID)
    {
        m_fCurrentHP -= 1.f;

        pNextState = CWhiteJetsu_BeatenState::Create(m_pNavigationCom, this, vDirection, 0.f);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_KAMUI == eHandleID)
    {
        m_fCurrentHP -= 1.f;

        pNextState = CWhiteJetsu_BeatenState::Create(m_pNavigationCom, this, vDirection, 0.f);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_KAMUI_END == eHandleID ||
                COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENSHURIKEN_END == eHandleID)
    {
        m_fCurrentHP -= 5.f;

        pNextState = CWhiteJetsu_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_BIGSHARK == eHandleID)
    {
        m_fCurrentHP -= 15.f;

        pNextState = CWhiteJetsu_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection);
        Set_Invincible(1.f);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_CHIDORI == eHandleID)
    {
        m_pGameManager->Change_Camera(static_cast<LEVEL>(m_pGameInstance->Get_LevelID()), 
            TEXT("Chidori_Action_Camera"), nullptr);

        m_fCurrentHP -= 15.f;
        fHitTime = 2.f;
        pNextState = CWhiteJetsu_ElectricShockState::Create(m_pNavigationCom, this);
        Set_Invincible(1.f);
    }

    if (m_fCurrentHP <= 0.f && false == m_IsPlayingDeadAnim)
    {
        m_IsPlayingDeadAnim = true;
        if (nullptr != pNextState)
        {
            pNextState->End();
            Safe_Release(pNextState);
        }
        pNextState = CWhiteJetsu_DeadState::Create(m_pNavigationCom, this);
    }
    
    Set_HitEffect(fHitTime, 1.f);

    Change_State(pNextState, false);
}

void CWhiteJetsu::Change_State(CWhiteJetsuState* pNextState, _bool bBlend)
{
    _bool IsBlend = m_pState->End();
    Safe_Release(m_pState);

    pNextState->Start(bBlend);
    m_pState = pNextState;

}

HRESULT CWhiteJetsu::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWhiteJetsu::Initialize(void* pArg)
{
    CGameObject::GAMEOBJECT_DESC	Desc{};
    Desc.fRotationPerSec = XMConvertToRadians(180.0f);
    Desc.fSpeedPerSec = 7.5f;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Position()))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_HPBar()))
        return E_FAIL;

    m_iNumMeshes = m_pModelCom->Get_NumMeshes();

    /* 상태 초기화 및 시작. */
    m_pGameManager->Add_TargetTransform(m_pTransformCom);

    m_pState = CWhiteJetsu_IdleState::Create(m_pNavigationCom, this);
    m_pState->Start(true);

    m_fCurrentHP = 100.f;
    m_fMaxHP = 100.f;

    Fade_Particle();

    return S_OK;
}

void CWhiteJetsu::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CWhiteJetsu::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    if(m_iShaderPassIdx != 0)
        Calc_HitEffectTime(fTimeDelta);
    /* 스테이트 업데이트. */
    Update_State(fTimeDelta);
    /* 스킬 쿨타임 업데이트*/
    Update_SkillCoolDown(fTimeDelta);

    _matrix PlayerMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    _vector PlayerTranslation = m_pTransformCom->Get_State(STATE::POSITION);
    PlayerTranslation += m_pTransformCom->Get_State(STATE::LOOK) * 0.4f;
    PlayerMatrix.r[3] = PlayerTranslation;

    m_pHandAttackColliderCom->Update(PlayerMatrix);

	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

}

void CWhiteJetsu::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    /* 제츠의 몸통 콜라이더를 콜리전 매니저에 등록 */
    m_pGameManager->Add_Object_ToCollision(TEXT("Monster_Body"), this, m_pColliderCom);
    m_pGameManager->Add_Collider_ToCollision(TEXT("Monster_Attack"), COLLIDER_HANDLE_ID::ENEMY_JETSU_ATTACK, m_pHandAttackColliderCom);

    m_pNavigationCom->Compute_Height(m_pTransformCom);

    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
    m_pGameInstance->Add_DebugComponent(m_pHandAttackColliderCom);
#endif

}

HRESULT CWhiteJetsu::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    for (_uint i = 0; i < m_iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(m_iShaderPassIdx)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CWhiteJetsu::Render_Shadow()
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
        if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }
    return S_OK;
}

_bool CWhiteJetsu::Use_Skill()
{
    if (m_fSkillTimeAcc >= m_fMaxSkillCoolDown)
    {
        m_fSkillTimeAcc = 0.f;
        return true;
    }
    else
        return false;
}

void CWhiteJetsu::Update_SkillCoolDown(_float fTimeDelta)
{
    m_fSkillTimeAcc += fTimeDelta;

    if (m_fSkillTimeAcc >= m_fMaxSkillCoolDown)
        m_fSkillTimeAcc = m_fMaxSkillCoolDown;
}

HRESULT CWhiteJetsu::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_WhiteJetsu"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Navigation */
    CNavigation::NAVIGATION_DESC Desc;
    Desc.iCurrentCellIndex = 0;
    XMStoreFloat3(&Desc.vPosition, m_pTransformCom->Get_State(STATE::POSITION));

    if (LEVEL::TUTORIAL == m_pGameManager->Get_NextLevel())
    {
        if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_Component_Navigation_Tutorial"),
            TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
            return E_FAIL;
    }
    else if (LEVEL::KONOHA_VILLAGE == m_pGameManager->Get_NextLevel())
    {
        if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Navigation_KonohaVillage_2"),
            TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
            return E_FAIL;
    }

    /* Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC OBBDesc{};

    OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
    OBBDesc.vSize = _float3(0.7f, 1.4f, 0.7f);
    OBBDesc.vCenter = _float3(0.f, 0.8f, 0.f);
    OBBDesc.isActive = true;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
        return E_FAIL;

    CBounding_Sphere::BOUNDING_SPHERE_DESC ColliderDesc{};

    ColliderDesc.fRadius = 0.7f;
    ColliderDesc.vCenter = _float3{ 0.f, 0.7f, 0.f };
    ColliderDesc.isActive = false;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_HandAttack"), reinterpret_cast<CComponent**>(&m_pHandAttackColliderCom), &ColliderDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CWhiteJetsu::Ready_Position()
{
    if (TRIGGER_TYPE::TUTORIAL_SPAWNER_01 == m_pGameManager->Get_CurrentTrigger())
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(7.f, 0.f, 3.f, 1.f));
    else if (TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_01 == m_pGameManager->Get_CurrentTrigger())
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(94.140f, 7.775f, -69.703f, 1.f));

    return S_OK;
}

HRESULT CWhiteJetsu::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fIntensity", &m_fIntensity, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEffectTimeAcc", &m_fEffectTimeAcc, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEffectTime", &m_fEffectTime, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

void CWhiteJetsu::Update_State(_float fTimeDelta)
{
    CWhiteJetsuState* pNextState = { nullptr };
    pNextState = m_pState->Update(fTimeDelta);

    if (nullptr != pNextState)
    {
        _bool IsBlend = m_pState->End();

        //현재 스테이트 날려버리기.
        Safe_Release(m_pState);

        //새로운 상태 시작하기. (내부적으로 플레이어 들게 됨)
        pNextState->Start(IsBlend);

        m_pState = pNextState;
    }
}

CWhiteJetsu* CWhiteJetsu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CWhiteJetsu* pInstance = new CWhiteJetsu(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : White Jetsu");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWhiteJetsu::Clone(void* pArg)
{
    CWhiteJetsu* pInstance = new CWhiteJetsu(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : White Jetsu");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWhiteJetsu::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pState);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pHandAttackColliderCom);
}