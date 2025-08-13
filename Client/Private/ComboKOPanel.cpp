#include "ComboKOPanel.h"

CComboKOPanel::CComboKOPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CPanel { pDevice, pContext, eObjectID }
{
}

CComboKOPanel::CComboKOPanel(const CComboKOPanel& rhs)
	: CPanel { rhs }
{
}

HRESULT CComboKOPanel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComboKOPanel::Initialize(void* pArg)
{
	return S_OK;
}

void CComboKOPanel::Priority_Update(_float fTimeDelta)
{
}

void CComboKOPanel::Update(_float fTimeDelta)
{
}

void CComboKOPanel::Late_Update(_float fTimeDelta)
{
}

HRESULT CComboKOPanel::Render()
{
	return S_OK;
}

HRESULT CComboKOPanel::Ready_Components()
{
	return S_OK;
}

HRESULT CComboKOPanel::Ready_Deicmals()
{
	/* Decimal 준비해줄 것 */
	return S_OK;
}

HRESULT CComboKOPanel::Ready_KO()
{
	/* KO 준비해줄 것 */
	return S_OK;
}

HRESULT CComboKOPanel::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

CComboKOPanel* CComboKOPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CComboKOPanel* pInstance = new CComboKOPanel(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : ComboKO panel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CComboKOPanel::Clone(void* pArg)
{
	CComboKOPanel* pInstance = new CComboKOPanel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : ComboKO panel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CComboKOPanel::Free()
{
	__super::Free();
}
