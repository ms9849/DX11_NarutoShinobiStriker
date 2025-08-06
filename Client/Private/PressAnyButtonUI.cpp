#include "PressAnyButtonUI.h"

#include "GameInstance.h"

CPressAnyButtonUI::CPressAnyButtonUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CUIObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CPressAnyButtonUI::CPressAnyButtonUI(const CPressAnyButtonUI& rhs)
    : CUIObject { rhs }
{
}

HRESULT CPressAnyButtonUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPressAnyButtonUI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CPressAnyButtonUI::Priority_Update(_float fTimeDelta)
{
}

void CPressAnyButtonUI::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(VK_SPACE))
        m_bTriggered = true;

    if (m_bTriggered)
        Play_Animation(fTimeDelta);
}

void CPressAnyButtonUI::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CPressAnyButtonUI::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

void CPressAnyButtonUI::Play_Animation(_float fTimeDelta)
{
    m_fTimeAcc += fTimeDelta;

    if (m_fTimeAcc >= 0.1f && m_iTextureIdx < 2)
    {
        m_iTextureIdx++;
        m_fTimeAcc = 0.f;
    }

    if (m_fTimeAcc > 1.5f)
        m_pGameInstance->Request_LevelChange();
}

HRESULT CPressAnyButtonUI::Ready_Components()
{
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_PressAnyButtonUI"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CPressAnyButtonUI::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pOrthogonalCom->Bind_ViewMatrix(m_pShaderCom, "g_ViewMatrix")))
        return E_FAIL;

    if (FAILED(m_pOrthogonalCom->Bind_ProjMatrix(m_pShaderCom, "g_ProjMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIdx)))
        return E_FAIL;

    return S_OK;
}

CPressAnyButtonUI* CPressAnyButtonUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CPressAnyButtonUI* pInstance = new CPressAnyButtonUI(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CPressAnyButtonUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPressAnyButtonUI::Clone(void* pArg)
{
    CPressAnyButtonUI* pInstance = new CPressAnyButtonUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CPressAnyButtonUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPressAnyButtonUI::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
