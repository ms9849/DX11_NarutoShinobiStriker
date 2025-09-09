#include "WhiteJetsu.h"

#include "GameInstance.h"
#include "WhiteJetsu_IdleState.h"

CWhiteJetsu::CWhiteJetsu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CEnemy { pDevice, pContext, eObjectID }
{
}

CWhiteJetsu::CWhiteJetsu(const CWhiteJetsu& rhs)
    : CEnemy{ rhs }
{
}

HRESULT CWhiteJetsu::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWhiteJetsu::Initialize(void* pArg)
{
    CGameObject::GAMEOBJECT_DESC	Desc{};
    Desc.fRotationPerSec = XMConvertToRadians(180.0f);
    Desc.fSpeedPerSec = 10.f;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_HPBar()))
        return E_FAIL;

   m_iNumMeshes = m_pModelCom->Get_NumMeshes();

    /* 상태 초기화 및 시작. */
    m_pState = CWhiteJetsu_IdleState::Create(m_pTransformCom, m_pModelCom);
    m_pState->Start(true);

    return S_OK;
}

void CWhiteJetsu::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CWhiteJetsu::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    /* 스테이트 업데이트. */
    Update_State(fTimeDelta);
}

void CWhiteJetsu::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
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

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
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

    return S_OK;
}

HRESULT CWhiteJetsu::Bind_ShaderResources()
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
}
