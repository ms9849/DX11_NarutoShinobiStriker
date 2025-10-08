#include "Mannequin.h"

#include "GameManager.h"
#include "GameInstance.h"

CMannequin::CMannequin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CContainerObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CMannequin::CMannequin(const CMannequin& rhs)
	: CContainerObject { rhs }
{
}

HRESULT CMannequin::Initialize_Prototype()
{
	return  S_OK;
}

HRESULT CMannequin::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_AllOutfits()))
		return E_FAIL;

	return S_OK;
}

void CMannequin::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CMannequin::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CMannequin::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CMannequin::Render()
{
	return S_OK;
}

HRESULT CMannequin::Ready_AllOutfits()
{
	return S_OK;
}

HRESULT CMannequin::Ready_Components()
{
	return S_OK;
}

HRESULT CMannequin::Bind_ShaderResources()
{
	return S_OK;
}

CMannequin* CMannequin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CMannequin* pInstance = new CMannequin(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed ! : CMannequin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMannequin::Clone(void* pArg)
{
	CMannequin* pInstance = new CMannequin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed ! : CMannequin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMannequin::Free()
{
	__super::Free();
}
