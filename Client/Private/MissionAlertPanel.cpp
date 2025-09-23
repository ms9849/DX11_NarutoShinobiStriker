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

void CMissionAlertPanel::AlertPanel_Start_FadeIn(const _wstring& strMissionText)
{
    m_strMissionText = strMissionText;
    m_fTimeAcc = 0.f;
    m_IsVisible = true;
    m_IsFadeIn = true;
    m_IsTriggered = true;
    m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT);
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet((m_fX - g_iWinSizeX / 2.f - m_fAnimationDist * (1 - m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc)), -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));
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

    return S_OK;
}

void CMissionAlertPanel::Priority_Update(_float fTimeDelta)
{
}

void CMissionAlertPanel::Update(_float fTimeDelta)
{
    m_fTimeAcc += fTimeDelta;

    if (m_fTimeAcc >= m_fLifeTime && m_IsTriggered == true)
    {
        m_IsFadeOut = true;
        m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT);
    }

    if (m_IsFadeIn)
        Play_Animation_FadeIn(fTimeDelta);

    if (m_IsFadeOut)
        Play_Animation_FadeOut(fTimeDelta);
}

void CMissionAlertPanel::Late_Update(_float fTimeDelta)
{
    if (false == m_IsVisible)
        return;

    /* 렌더러에 자기자신 추가 */
    m_pGameInstance->Add_RenderGroup(RENDER::UI, this);

    /* 폰트 추가 */
    _float4 vPosition = m_pTransformCom->Get_State_Float4(STATE::POSITION);
    _float fAlpha = 1.f;

    if (m_IsFadeIn)
        fAlpha = m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc;

    else if (m_IsFadeOut)
        fAlpha = 1 - (m_fFadeOutTimeAcc / m_fFadeOutMaxTimeAcc);

    m_pFontCom->Bind_Resources(m_strMissionText.c_str(), _float2{vPosition.x, vPosition.y}, true,
        1.f, XMVectorSet(1.f, 1.f, 1.f, fAlpha));

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

    if (m_iShaderPassIdx == ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT) && m_IsFadeIn)
    {
        _float fAlphaValue = m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc;
        m_pShaderCom->Bind_RawValue("g_Alpha", &fAlphaValue, sizeof(_float));
    }

    else if (m_iShaderPassIdx == ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT) && m_IsFadeOut)
    {
        _float fAlphaValue = 1 - (m_fFadeOutTimeAcc / m_fFadeOutMaxTimeAcc);
        m_pShaderCom->Bind_RawValue("g_Alpha", &fAlphaValue, sizeof(_float));
    }

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
        m_IsFadeIn = false;
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
        m_IsFadeOut = false;
        m_fFadeOutTimeAcc = 0.f;
        m_fTimeAcc = 0.f;
        m_IsTriggered = false;
        m_IsVisible = false;
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

    Safe_Release(m_pFontCom);
}
