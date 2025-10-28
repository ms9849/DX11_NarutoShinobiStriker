#include "Effect_Icon.h"

#include "GameInstance.h"
#include "GameManager.h"

CEffect_Icon::CEffect_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CEffect_Icon::CEffect_Icon(const CEffect_Icon& Prototype)
    : CGameObject { Prototype }
{
}

void CEffect_Icon::Set_Desc(void* pArg)
{
    m_pTargetTransform = static_cast<EFFECT_ICON_DESC*>(pArg)->pTargetTransform;
    Safe_AddRef(m_pTargetTransform);

    m_pTransformCom->Set_State(STATE::POSITION, m_pTargetTransform->Get_State(STATE::POSITION));
}

HRESULT CEffect_Icon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_Icon::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CEffect_Icon::Priority_Update(_float fTimeDelta)
{
}

void CEffect_Icon::Update(_float fTimeDelta)
{
    /* 카메라랑 같은 시점 바라보게끔 설정 */
    _float4x4 CameraWorld = *m_pGameInstance->Get_PipeLine_InverseFloat4x4(D3DTS::VIEW);

    m_pTransformCom->Set_State(STATE::RIGHT, *reinterpret_cast<_vector*>(&CameraWorld.m[0]));
    m_pTransformCom->Set_State(STATE::UP, *reinterpret_cast<_vector*>(&CameraWorld.m[1]));
    m_pTransformCom->Set_State(STATE::LOOK, *reinterpret_cast<_vector*>(&CameraWorld.m[2]));

    m_fTimeAcc += fTimeDelta;

    if (m_fTimeAcc >= 0.2f)
    {
        m_IsDead = true;
        m_fTimeAcc = 0.f;
        m_pTransformCom->Set_Scale(0.5f, 0.5f, 1.0f);
        Safe_Release(m_pTargetTransform);
    }
    else
        m_pTransformCom->Set_Scale(m_fTimeAcc * 4.f + 0.5f, m_fTimeAcc * 4.f + 0.5f, 1.0f);
}

void CEffect_Icon::Late_Update(_float fTimeDelta)
{
    if (true == m_IsDead)
        return;

    m_pTransformCom->Set_State(STATE::POSITION, m_pTargetTransform->Get_State(STATE::POSITION));

    m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CEffect_Icon::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_MASK))))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Icon::Ready_Components()
{
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SkillCoolDownEffect"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    CPooling::POOLING_DESC Desc = {};
    Desc.strPoolingTag = TEXT("Effect_Icon");

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Pooling"),
        reinterpret_cast<CComponent**>(&m_pPoolingCom), &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Icon::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    return S_OK;
}

CEffect_Icon* CEffect_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CEffect_Icon* pInstance = new CEffect_Icon(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CEffect_Icon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_Icon::Clone(void* pArg)
{
    CEffect_Icon* pInstance = new CEffect_Icon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CEffect_Icon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_Icon::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pPoolingCom);
    if(nullptr != m_pTargetTransform)
        (m_pTargetTransform);
}
