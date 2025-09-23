#include "DialogUI.h"

#include "GameInstance.h"
#include "GameManager.h"

CDialogUI::CDialogUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CUIObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
    , m_pGameManager { CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

CDialogUI::CDialogUI(const CDialogUI& rhs)
    : CUIObject { rhs }
    , m_pGameManager{ CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

HRESULT CDialogUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDialogUI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_strNameFontText = TEXT("하타케 카카시");
    m_strFontText = TEXT("코딩하기싫다아아아아");
    m_IsVisible = false;

    return S_OK;
}

void CDialogUI::Priority_Update(_float fTimeDelta)
{
}

void CDialogUI::Update(_float fTimeDelta)
{
}

void CDialogUI::Late_Update(_float fTimeDelta)
{
    if (false == m_IsVisible)
        return;

    m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
    
    /* 이름 추가 */
    _float4 vPosition = m_pTransformCom->Get_State_Float4(STATE::POSITION);

    m_pNameFontCom->Bind_Resources(m_strNameFontText.c_str(), _float2{ vPosition.x - 240, vPosition.y + 55 }, false,
        0.7f, XMVectorSet(1.f, 1.f, 1.f, 1.f));

    m_pGameInstance->Add_Font(m_pNameFontCom);

    /* 폰트 추가 */
    vPosition = m_pTransformCom->Get_State_Float4(STATE::POSITION);

    m_pFontCom->Bind_Resources(m_strFontText.c_str(), _float2{ vPosition.x - 240, vPosition.y + 15 }, false,
        0.7f, XMVectorSet(0.f, 0.f, 0.f, 1.f));

    m_pGameInstance->Add_Font(m_pFontCom);
}

HRESULT CDialogUI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CDialogUI::Ready_Components()
{
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_DialogUI"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Font"),
        TEXT("Com_Font"), reinterpret_cast<CComponent**>(&m_pFontCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Font"),
        TEXT("Com_NameFont"), reinterpret_cast<CComponent**>(&m_pNameFontCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CDialogUI::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;


    return S_OK;
}

CDialogUI* CDialogUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CDialogUI* pInstance = new CDialogUI(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CDialogUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDialogUI::Clone(void* pArg)
{
    CDialogUI* pInstance = new CDialogUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CDialogUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDialogUI::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pFontCom);
    Safe_Release(m_pNameFontCom);
    Safe_Release(m_pGameManager);
}
