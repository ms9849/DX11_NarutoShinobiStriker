#include "Dummy.h"

#include "Pooling.h"

CDummy::CDummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID) :
	CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CDummy::CDummy(const CDummy& Prototype) :
	CGameObject { Prototype }
{
}

HRESULT CDummy::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDummy::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

    return S_OK;
}

void CDummy::Priority_Update(_float fTimeDelta)
{

}

void CDummy::Update(_float fTimeDelta)
{

}

void CDummy::Late_Update(_float fTimeDelta)
{
	/* 테스트용. */
	m_fLife += fTimeDelta;

	if (m_fLife >= 15.f)
	{
		m_IsDead = true;
		m_fLife = 0.f;
	}
}

HRESULT CDummy::Render()
{
    return S_OK;
}

void CDummy::Set_Desc(void* pDesc)
{
	DUMMY_DESC* pDummyDesc = static_cast<DUMMY_DESC*>(pDesc);
}

HRESULT CDummy::Ready_Components()
{
	/* Com_Pooling */
	CPooling::POOLING_DESC Desc = {};
	Desc.strPoolingTag = TEXT("Object_Dummy");

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Pooling"),
		reinterpret_cast<CComponent**>(&m_pPoolingCom), &Desc)))
		return E_FAIL;

    return S_OK;
}

CDummy* CDummy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CDummy* pInstance = new CDummy(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CDummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy::Clone(void* pArg)
{
	CDummy* pInstance = new CDummy(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy::Free()
{
    __super::Free();

	Safe_Release(m_pPoolingCom);
}
