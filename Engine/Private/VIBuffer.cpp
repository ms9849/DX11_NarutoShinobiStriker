#include "VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) 
	: CComponent { pDevice , pContext }
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: CComponent { rhs }
	, m_pVB { rhs.m_pVB }
	, m_pIB { rhs.m_pIB }
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
	return S_OK;
}

void CVIBuffer::Free()
{
	__super::Free();

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
