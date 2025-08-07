#include "DecimalUI.h"

#include "GameInstance.h"

CDecimalUI::CDecimalUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CUIObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CDecimalUI::CDecimalUI(const CDecimalUI& rhs)
	: CUIObject { rhs }
{
}

HRESULT CDecimalUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDecimalUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iTextureIdx = 0;

	return S_OK;
}

void CDecimalUI::Priority_Update(_float fTimeDelta)
{
	int a = 10; // test
}

void CDecimalUI::Update(_float fTimeDelta)
{
}

void CDecimalUI::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CDecimalUI::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CDecimalUI::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_DecimalUI"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDecimalUI::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

CDecimalUI* CDecimalUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CDecimalUI* pInstance = new CDecimalUI(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : Decimal UI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDecimalUI::Clone(void* pArg)
{
	CDecimalUI* pInstance = new CDecimalUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : Decimal UI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecimalUI::Free()
{
	__super::Free();
}
