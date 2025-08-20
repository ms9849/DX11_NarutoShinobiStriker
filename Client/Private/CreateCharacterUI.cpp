#include "CreateCharacterUI.h"

#include "GameInstance.h"

CCreateCharacterUI::CCreateCharacterUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CUIObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CCreateCharacterUI::CCreateCharacterUI(const CCreateCharacterUI& rhs)
    : CUIObject { rhs }
{
}

HRESULT CCreateCharacterUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCreateCharacterUI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_bTriggered = true;
    m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT);

    return S_OK;
}

void CCreateCharacterUI::Priority_Update(_float fTimeDelta)
{
}

void CCreateCharacterUI::Update(_float fTimeDelta)
{
    if (m_bTriggered)
        Play_Animation_FadeIn(fTimeDelta);
}

void CCreateCharacterUI::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CCreateCharacterUI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CCreateCharacterUI::Play_Animation_FadeIn(_float fTimeDelta)
{
    m_fTimeAcc += fTimeDelta;

    if (m_fTimeAcc < m_fMaxTimeAcc)
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet((m_fX - g_iWinSizeX / 2.f - m_fAnimationDist * (1 - m_fTimeAcc / m_fMaxTimeAcc)), -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));
    }
    /* 애니메이션 종료 */
    else if (m_fTimeAcc >= m_fMaxTimeAcc)
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - g_iWinSizeX / 2.f, -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));
        m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI);
        m_bTriggered = false;
        m_fTimeAcc = 0.f;
    }
}

HRESULT CCreateCharacterUI::Ready_Components()
{
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_Component_Texture_CreateCharacterUI"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CCreateCharacterUI::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    if (m_iShaderPassIdx == ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT))
        m_pShaderCom->Bind_Float("g_Alpha", m_fTimeAcc / m_fMaxTimeAcc);

    return S_OK;
}

CCreateCharacterUI* CCreateCharacterUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CCreateCharacterUI* pInstance = new CCreateCharacterUI(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CreateChracterUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCreateCharacterUI::Clone(void* pArg)
{
    CCreateCharacterUI* pInstance = new CCreateCharacterUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CreateChracterUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCreateCharacterUI::Free()
{
    __super::Free();
}
