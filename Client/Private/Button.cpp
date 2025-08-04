#include "Button.h"

#include "GameInstance.h"

CButton::CButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CUIObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CButton::CButton(const CButton& rhs)
	: CUIObject { rhs }
{
}

HRESULT CButton::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CButton::Initialize(void* pArg)
{
	/* 부모로 받아온 녀석의 상대적인 위치로 세팅해줘야 할 것.. */

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	/* 움직이는 UI는 없을 듯 함.. */
	m_rcButton.left = (LONG)m_fX - (LONG)(m_pTransformCom->Get_Scale().x / 2.f);
	m_rcButton.top = (LONG)m_fY - (LONG)(m_pTransformCom->Get_Scale().y / 2.f);
	m_rcButton.right = (LONG)m_fX + (LONG)(m_pTransformCom->Get_Scale().x / 2.f);
	m_rcButton.bottom = (LONG)m_fY + (LONG)(m_pTransformCom->Get_Scale().y / 2.f);

	return S_OK;
}

void CButton::Priority_Update(_float fTimeDelta)
{
}

void CButton::Update(_float fTimeDelta)
{
}

void CButton::Late_Update(_float fTimeDelta)
{
}

HRESULT CButton::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CButton::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"), 
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CButton::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pOrthogonalCom->Bind_ViewMatrix(m_pShaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_pOrthogonalCom->Bind_ProjMatrix(m_pShaderCom, "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

void CButton::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
