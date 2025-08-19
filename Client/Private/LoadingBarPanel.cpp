#include "LoadingBarPanel.h"

#include "GameInstance.h"
#include "ProgressBarUI.h"

CLoadingBarPanel::CLoadingBarPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CPanel{ pDevice, pContext, eObjectID }
{
}

CLoadingBarPanel::CLoadingBarPanel(const CLoadingBarPanel& rhs)
    : CPanel{ rhs }
{
}

HRESULT CLoadingBarPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLoadingBarPanel::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_ProgressBar()))
        return E_FAIL;

    return S_OK;
}

void CLoadingBarPanel::Priority_Update(_float fTimeDelta)
{
}

void CLoadingBarPanel::Update(_float fTimeDelta)
{
}

void CLoadingBarPanel::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CLoadingBarPanel::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CLoadingBarPanel::Set_LoadingProgress(_float fProgress)
{
    static_cast<CProgressBarUI*>(m_Childs[0])->Set_Progress(fProgress);
}

void CLoadingBarPanel::Set_MaxLoadingProgress(_float fMaxProgress)
{
    static_cast<CProgressBarUI*>(m_Childs[0])->Set_MaxProgress(fMaxProgress);
}

HRESULT CLoadingBarPanel::Ready_Components()
{
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingBarPanel"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoadingBarPanel::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoadingBarPanel::Ready_ProgressBar()
{
    UIOBJECT_DESC Desc = CUIObject::CreateDesc(m_fX, m_fY, m_fZ - 0.05f, g_iWinSizeX - 300, 10, 1, 0.f);

    CProgressBarUI* pProgressBar = static_cast<CProgressBarUI*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC),
        TEXT("Prototype_GameObject_ProgressBarUI"), &Desc));

    m_pGameInstance->Add_Clone_ToLayer(pProgressBar, ENUM_CLASS(LEVEL::LOADING), TEXT("Layer_UI"));
    m_Childs.push_back(pProgressBar);
    Safe_AddRef(pProgressBar);

    return S_OK;
}

CLoadingBarPanel* CLoadingBarPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CLoadingBarPanel* pInstance = new CLoadingBarPanel(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed! : CLoadingBarPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLoadingBarPanel::Clone(void* pArg)
{
    CLoadingBarPanel* pInstance = new CLoadingBarPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed! : CLoadingBarPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLoadingBarPanel::Free()
{
    __super::Free();
}
