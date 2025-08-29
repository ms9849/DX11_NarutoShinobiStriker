#include "MissionAlertPanel.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Font.h"

CMissionAlertPanel::CMissionAlertPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CPanel { pDevice, pContext, eObjectID }
{
}

CMissionAlertPanel::CMissionAlertPanel(const CMissionAlertPanel& rhs)
    : CPanel { rhs }
{
}

HRESULT CMissionAlertPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMissionAlertPanel::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_bTriggered = true;
    m_bFadeIn = true;
    m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT);
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet((m_fX - g_iWinSizeX / 2.f - m_fAnimationDist * (1 - m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc)), -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));

    return S_OK;
}

void CMissionAlertPanel::Priority_Update(_float fTimeDelta)
{
}

void CMissionAlertPanel::Update(_float fTimeDelta)
{
    m_fTimeAcc += fTimeDelta;

    if (m_fTimeAcc >= m_fLifeTime && m_bTriggered == true)
    {
        m_bFadeOut = true;
        m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT);
    }

    if (m_bFadeIn)
        Play_Animation_FadeIn(fTimeDelta);

    if (m_bFadeOut)
        Play_Animation_FadeOut(fTimeDelta);
}

void CMissionAlertPanel::Late_Update(_float fTimeDelta)
{
    if(m_bVisible)
        m_pGameInstance->Add_RenderGroup(RENDER::UI, this);

    m_pGameInstance->Add_Font(m_pFontCom);
}

HRESULT CMissionAlertPanel::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMissionAlertPanel::Ready_Components()
{
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Font"),
        TEXT("Com_Font"), reinterpret_cast<CComponent**>(&m_pFontCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Prototype_Component_Texture_MissionAlertPanel"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMissionAlertPanel::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    if (m_iShaderPassIdx == ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT) && m_bFadeIn)
        m_pShaderCom->Bind_Float("g_Alpha", m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc);

    else if (m_iShaderPassIdx == ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT) && m_bFadeOut)
        m_pShaderCom->Bind_Float("g_Alpha", 1 - (m_fFadeOutTimeAcc / m_fFadeOutMaxTimeAcc));

    return S_OK;
}

void CMissionAlertPanel::Play_Animation_FadeIn(_float fTimeDelta)
{
    m_fFadeInTimeAcc += fTimeDelta;

    if (m_fFadeInTimeAcc >= m_fFadeInMaxTimeAcc)
        m_fFadeInTimeAcc = m_fFadeInMaxTimeAcc;

    else if (m_fFadeInTimeAcc < m_fFadeInMaxTimeAcc)
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet((m_fX - g_iWinSizeX / 2.f - m_fAnimationDist * (1 - m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc)), -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));

    /* 애니메이션 종료 */
    if (m_fFadeInTimeAcc >= m_fFadeInMaxTimeAcc)
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - g_iWinSizeX / 2.f, -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));
        m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI);
        m_bFadeIn = false;
        m_fFadeInTimeAcc = 0.f;
    }
}

void CMissionAlertPanel::Play_Animation_FadeOut(_float fTimeDelta)
{
    m_fFadeOutTimeAcc += fTimeDelta;

    if (m_fFadeOutTimeAcc < m_fFadeOutMaxTimeAcc)
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet((m_fX - g_iWinSizeX / 2.f - m_fAnimationDist * (m_fFadeOutTimeAcc / m_fFadeOutMaxTimeAcc)), -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));
    }
    /* 애니메이션 종료 */
    else if (m_fFadeOutTimeAcc >= m_fFadeOutMaxTimeAcc)
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - g_iWinSizeX / 2.f, -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));
        m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI);
        m_bFadeOut = false;
        m_fFadeOutTimeAcc = 0.f;
        m_fTimeAcc = 0.f;
        m_bTriggered = false;
        m_bVisible = false;
    }
}

CMissionAlertPanel* CMissionAlertPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CMissionAlertPanel* pInstance = new CMissionAlertPanel(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Clone Failed : CMissionAlertPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMissionAlertPanel::Clone(void* pArg)
{
    CMissionAlertPanel* pInstance = new CMissionAlertPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CMissionAlertPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMissionAlertPanel::Free()
{
    __super::Free();
}
