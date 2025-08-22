#include "Props.h"


CProps::CProps(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CProps::CProps(const CProps& rhs)
	: CGameObject { rhs }
{
}

HRESULT CProps::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CProps::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CProps::Priority_Update(_float fTimeDelta)
{
}

void CProps::Update(_float fTimeDelta)
{
}

void CProps::Late_Update(_float fTimeDelta)
{
}

HRESULT CProps::Render()
{
	return E_NOTIMPL;
}

CProps* CProps::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CProps* pInstance = new CProps(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CProps");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProps::Clone(void* pArg)
{
	CProps* pInstance = new CProps(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProps::Free()
{
}
