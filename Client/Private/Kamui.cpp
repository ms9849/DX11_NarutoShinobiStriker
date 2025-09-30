#include "Kamui.h"

#include "GameInstance.h"
#include "GameManager.h"

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

    /* 포지션, 룩 세팅. */
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&pDesc->vPosition));
    m_pTransformCom->LookAt(m_pTransformCom->Get_State(STATE::POSITION) -1.f * XMLoadFloat3(&pDesc->vLook));

    return S_OK;
}

void CKamui::Priority_Update(_float fTimeDelta)
{
}

void CKamui::Update(_float fTimeDelta)
{
    m_fTimeAcc += fTimeDelta;
    m_fAttackCoolDown += fTimeDelta;

    if (true == m_isFinal)
        m_IsDead = true;

    if (m_fTimeAcc >= m_fLifeTime)
        m_isFinal = true;

    m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
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

    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CKamui::Render()
{
    return S_OK;
}

HRESULT CKamui::Ready_Components()
{
    ///* Com_Model */
    //if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototoype_Component_Model_Kamui"),
    //    TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    //    return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    CBounding_OBB::BOUNDING_OBB_DESC		OBBDesc{};

    OBBDesc.vSize = { 15.f, 10.f, 15.f };
    OBBDesc.vCenter = { 0.f, 5.f, 0.f };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CKamui::Bind_ShaderResources()
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
}
