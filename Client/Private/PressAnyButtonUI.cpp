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
    if (m_pGameInstance->Key_Down(DIK_SPACE))
        m_bTriggered = true;

    if (m_bTriggered)
        Play_Animation_FadeOut(fTimeDelta);
}

void CPressAnyButtonUI::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CPressAnyButtonUI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CPressAnyButtonUI::Play_Animation_FadeOut(_float fTimeDelta)
{
    m_fTimeAcc += fTimeDelta;

    /* 애니메이션 재생 */
    if (m_fTimeAcc >= 0.05f && m_iTextureIdx < 2)
    {
        m_iTextureIdx++;
        m_fTimeAcc = 0.f;
    }

    /* 셰이더로 흐려지는 효과 + 커지는 효과 */
    if (m_fTimeAcc < m_fMaxTimeAcc && m_iTextureIdx == 2)
    {
        m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT);
        /* 원래 사이즈에서 m_fMaxTimeAcc 까지 커짐. */
        m_pTransformCom->Set_Scale(m_fSizeX * (1 + m_fTimeAcc * 3.f), m_fSizeY * (1 + m_fTimeAcc * 5.f), 1.f);
    }

    else if (m_fTimeAcc >= m_fMaxTimeAcc && m_iTextureIdx == 2)
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
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    if (m_iShaderPassIdx == ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT))
    {
        _float fAlphaValue = (m_fMaxTimeAcc - m_fTimeAcc) / m_fMaxTimeAcc;
        m_pShaderCom->Bind_RawValue("g_Alpha", &fAlphaValue, sizeof(_float));
    }

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
}
