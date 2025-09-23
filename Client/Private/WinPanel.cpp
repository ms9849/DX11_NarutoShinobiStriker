#include "WinPanel.h"

#include "GameInstance.h"
#include "GameManager.h"

CWinPanel::CWinPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CPanel{ pDevice, pContext, eObjectID }
{
}

CWinPanel::CWinPanel(const CWinPanel& rhs)
    : CPanel{ rhs }
{
}

void CWinPanel::WinPanel_Start_FadeIn()
{
    if (true == m_IsFadeInFinished)
        return;

    m_IsVisible = true;
    m_IsFadeIn = true;
    m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT);
}

HRESULT CWinPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWinPanel::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CWinPanel::Priority_Update(_float fTimeDelta)
{
}

void CWinPanel::Update(_float fTimeDelta)
{
    if (m_IsFadeIn)
        Play_Animation_FadeIn(fTimeDelta);
}

void CWinPanel::Late_Update(_float fTimeDelta)
{
    if (false == m_IsVisible)
        return;

    /* 렌더러에 자기자신 추가 */
    m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CWinPanel::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CWinPanel::Ready_Components()
{
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Prototype_Component_Texture_WinPanel"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CWinPanel::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    if (m_iShaderPassIdx == ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT) && m_IsFadeIn)
    {
        _float fAlphaValue = m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc;
        m_pShaderCom->Bind_RawValue("g_Alpha", &fAlphaValue, sizeof(_float));
    }

    return S_OK;
}

void CWinPanel::Play_Animation_FadeIn(_float fTimeDelta)
{
    m_fFadeInTimeAcc += fTimeDelta;

    if (m_fFadeInTimeAcc > m_fFadeInMaxTimeAcc)
        m_fFadeInTimeAcc = m_fFadeInMaxTimeAcc;

    m_pTransformCom->Set_Scale(m_fSizeX * (m_fMaxScale - 0.5f * m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc), m_fSizeY * (m_fMaxScale - 0.5f * m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc), m_fZ);

    /* 애니메이션 종료 */
    if (m_fFadeInTimeAcc >= m_fFadeInMaxTimeAcc)
    {
        m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, m_fZ);
        m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI);
        m_IsFadeIn = false;
        m_fFadeInTimeAcc = 0.f;
        m_IsFadeInFinished = true;
    }
}

CWinPanel* CWinPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CWinPanel* pInstance = new CWinPanel(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CWinPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWinPanel::Clone(void* pArg)
{
    CWinPanel* pInstance = new CWinPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CWinPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWinPanel::Free()
{
    __super::Free();
}
