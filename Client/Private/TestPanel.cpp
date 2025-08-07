#include "TestPanel.h"

#include "TestButton.h"
#include "GameInstance.h"

CTestPanel::CTestPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CPanel { pDevice, pContext, eObjectID }
{
}

CTestPanel::CTestPanel(const CTestPanel& rhs)
	: CPanel { rhs }
{
}

HRESULT CTestPanel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTestPanel::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Button()))
        return E_FAIL;

    return S_OK;
}

void CTestPanel::Priority_Update(_float fTimeDelta)
{
}

void CTestPanel::Update(_float fTimeDelta)
{
}

void CTestPanel::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CTestPanel::Render()
{
    __super::Render();

    return S_OK;
}

HRESULT CTestPanel::Ready_Components()
{
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_Panel"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

	return S_OK;
}

HRESULT CTestPanel::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTestPanel::Ready_Button()
{
    CUIObject::UIOBJECT_DESC Desc;
    Desc.fSizeX = 200;
    Desc.fSizeY = 100;
    Desc.fX = m_fX;
    Desc.fY = m_fY + 100;
    Desc.fZ = m_fZ - 0.05f;
    Desc.fAngle = m_fAngle;

    CButton* pButton = static_cast<CTestButton*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_UI_TestButton"), &Desc));
    if (nullptr == pButton)
        return E_FAIL;

    m_pGameInstance->Add_Clone_ToLayer(pButton, ENUM_CLASS(LEVEL::LOGO), TEXT("Layer_UI"));
    m_Childs.push_back(pButton);
    Safe_AddRef(pButton);

	return S_OK;
}

CTestPanel* CTestPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CTestPanel* pInstance = new CTestPanel(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : TestPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTestPanel::Clone(void* pArg)
{
    CTestPanel* pInstance = new CTestPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : TestPanel");
        Safe_Release(pInstance);
    }

	return pInstance;
}

void CTestPanel::Free()
{
    __super::Free();
}
