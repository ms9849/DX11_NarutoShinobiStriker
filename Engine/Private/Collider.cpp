#include "Collider.h"

#include "Bounding_AABB.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CCollider::CCollider(const CCollider& Prototype)
	: CComponent{ Prototype }
	, m_eType{ Prototype.m_eType }
{

}

HRESULT CCollider::Initialize_Prototype(COLLIDER eType)
{
	m_eType = eType;


	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{

	switch (m_eType)
	{
	case COLLIDER::AABB:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pArg);
		break;
	case COLLIDER::OBB:
		// m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pArg);
		break;
	case COLLIDER::SPHERE:
		// m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pArg);
		break;
	}

	return S_OK;
}

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	Safe_Release(m_pBounding);
}
