#include "LogoPanel.h"

#include "TitleUI.h"
#include "PressAnyButtonUI.h"

#include "GameInstance.h"

CLogoPanel::CLogoPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CPanel{ pDevice, pContext, eObjectID }
{
}

CLogoPanel::CLogoPanel(const CLogoPanel& rhs)
	: CPanel { rhs }
{
}

HRESULT CLogoPanel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLogoPanel::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if(FAILED(Ready_LogoUIs()))
		return E_FAIL;

	return S_OK;
}

void CLogoPanel::Priority_Update(_float fTimeDelta)
{
}

void CLogoPanel::Update(_float fTimeDelta)
{
}

void CLogoPanel::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CLogoPanel::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CLogoPanel::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_LogoPanel"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLogoPanel::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLogoPanel::Ready_LogoUIs()
{
	CUIObject::UIOBJECT_DESC Desc;
	/* Title */

	Desc = CUIObject::CreateDesc(m_fX, m_fY / 2.f + 50, m_fZ - 0.05f, 800.f, 200.f, 0, m_fAngle);

	CTitleUI* pTitle = static_cast<CTitleUI*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_TitleUI"), &Desc));
	if (nullptr == pTitle)
		return E_FAIL;

	m_pGameInstance->Add_Clone_ToLayer(pTitle, ENUM_CLASS(LEVEL::LOGO), TEXT("Layer_UI"));
	m_Childs.push_back(pTitle);
	Safe_AddRef(pTitle);

	/* Press Any Button */
	Desc = CUIObject::CreateDesc(m_fX, m_fY / 2.f + 350, m_fZ - 0.1f, 600.f, 100.f, 0, m_fAngle);

	CPressAnyButtonUI* pPressAnyButton = static_cast<CPressAnyButtonUI*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_PressAnyButtonUI"), &Desc));
	if (nullptr == pPressAnyButton)
		return E_FAIL;

	m_pGameInstance->Add_Clone_ToLayer(pPressAnyButton, ENUM_CLASS(LEVEL::LOGO), TEXT("Layer_UI"));
	m_Childs.push_back(pPressAnyButton);
	Safe_AddRef(pPressAnyButton);

	return S_OK;
}

CLogoPanel* CLogoPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CLogoPanel* pInstance = new CLogoPanel(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed! : LogoPanel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLogoPanel::Clone(void* pArg)
{
	CLogoPanel* pInstance = new CLogoPanel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed! : LogoPanel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogoPanel::Free()
{
	__super::Free();
}
