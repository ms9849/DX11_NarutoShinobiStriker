#include "ModelPanel.h"

#include "GameInstance.h"

#include "ModelSelectButtonUI.h"
#include "ModelDecideButtonUI.h"

CModelPanel::CModelPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CPanel { pDevice, pContext, eObjectID }
{
}

CModelPanel::CModelPanel(const CModelPanel& rhs)
    : CPanel{ rhs }
{
}

HRESULT CModelPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CModelPanel::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Buttons()))
        return E_FAIL;

    if (FAILED(Ready_DecideButton()))
        return E_FAIL;

    m_iFocusedNum = -1;

    return S_OK;
}

void CModelPanel::Priority_Update(_float fTimeDelta)
{
}

void CModelPanel::Update(_float fTimeDelta)
{
    Key_Input();
}

void CModelPanel::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CModelPanel::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CModelPanel::Change_FocusedButton(_int iNum)
{
    if(m_iFocusedNum >= 0 && m_iFocusedNum <= 8)
        static_cast<CButton*>(m_Childs[m_iFocusedNum])->Toggle_Focus();
    
    if (iNum > 8)
        iNum = 0;
    else if (iNum < 0)
        iNum = 8;

    if (iNum >= 0 && iNum <= 8)
        static_cast<CButton*>(m_Childs[iNum])->Toggle_Focus();
        

    m_iFocusedNum = iNum;
}

void CModelPanel::Change_SelectType_ToParts()
{
    // SELECT TYPE PARTS로 변경
}

void CModelPanel::Change_Outfits()
{
     // SELECT TYPE 변경, 혹은 외형 변경
}

void CModelPanel::Key_Input()
{
    if (m_pGameInstance->Key_Down(DIK_DOWN))
    {
        Change_FocusedButton(m_iFocusedNum + 1);
    }

    if (m_pGameInstance->Key_Down(DIK_UP))
    {
        Change_FocusedButton(m_iFocusedNum - 1);
    }

    if (m_pGameInstance->Key_Down(DIK_SPACE))
    {
        Change_Outfits();
    }

    if (m_pGameInstance->Key_Down(DIK_ESCAPE))
    {
        Change_SelectType_ToParts();
    }
}

HRESULT CModelPanel::Ready_Components()
{
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_Component_Texture_ModelPanel"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CModelPanel::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

HRESULT CModelPanel::Ready_Buttons()
{
    UIOBJECT_DESC Desc;
    CModelSelectButtonUI* pModelSelectButton;

    /* 테스트용 모델 외형 결정 버튼 */
    for (_uint i = 0; i < 8; ++i)
    {
        Desc = CUIObject::CreateDesc(g_iWinSizeX / 2.f - 300.f, g_iWinSizeY / 2.f - 100.f + 47.f * i, m_fZ - 0.05f, 380.f, 40.f, 0, 0.f);

        pModelSelectButton = static_cast<CModelSelectButtonUI*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_GameObject_ModelSelectButtonUI"), &Desc));
        m_pGameInstance->Add_Clone_ToLayer(pModelSelectButton, ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Layer_UI"));
    
        m_Childs.push_back(pModelSelectButton);
        Safe_AddRef(pModelSelectButton);
    }

    return S_OK;
}

HRESULT CModelPanel::Ready_DecideButton()
{
    UIOBJECT_DESC Desc;

    /* 테스트용 모델 외형 결정 버튼 */
    Desc = CUIObject::CreateDesc(g_iWinSizeX / 2.f - 300.f, g_iWinSizeY / 2.f + 300.f, m_fZ - 0.05f, 250.f, 50.f, 0, 0.f);

    CModelDecideButtonUI* pModelDecideButton = static_cast<CModelDecideButtonUI*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_GameObject_ModelDecideButtonUI"), &Desc));
    m_pGameInstance->Add_Clone_ToLayer(pModelDecideButton, ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Layer_UI"));

    m_Childs.push_back(pModelDecideButton);
    Safe_AddRef(pModelDecideButton);

    return S_OK;
}

CModelPanel* CModelPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CModelPanel* pInstance = new CModelPanel(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CModelPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CModelPanel::Clone(void* pArg)
{
    CModelPanel* pInstance = new CModelPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CModelPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CModelPanel::Free()
{
    __super::Free();
}
