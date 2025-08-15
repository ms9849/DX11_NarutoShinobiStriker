#include "RotationIconUI.h"

#include "GameInstance.h"

CRotationIconUI::CRotationIconUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CUIObject { pDevice , pContext, ENUM_CLASS(eObjectID) }
{
}

CRotationIconUI::CRotationIconUI(const CRotationIconUI& rhs)
    : CUIObject { rhs }
{
}

HRESULT CRotationIconUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRotationIconUI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CRotationIconUI::Priority_Update(_float fTimeDelta)
{
}

void CRotationIconUI::Update(_float fTimeDelta)
{
    Play_Animation_Rotate(fTimeDelta);
}

void CRotationIconUI::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CRotationIconUI::Render()
{
    __super::Render();

    return S_OK;
}

void CRotationIconUI::Play_Animation_Rotate(_float fTimeDelta)
{
    m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::LOOK), fTimeDelta);
}

HRESULT CRotationIconUI::Ready_Components()
{
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_RotationIconUI"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CRotationIconUI::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

CRotationIconUI* CRotationIconUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CRotationIconUI* pInstance = new CRotationIconUI(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CRotationIconUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRotationIconUI::Clone(void* pArg)
{
    CRotationIconUI* pInstance = new CRotationIconUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CRotationIconUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRotationIconUI::Free()
{
    __super::Free();
}
