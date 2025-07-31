#include "VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) 
	: CComponent { pDevice , pContext }
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: CComponent { rhs }
	, m_pVB { rhs.m_pVB }
	, m_pIB { rhs.m_pIB }
	, m_pVertexPositions{ rhs.m_pVertexPositions }
	, m_iNumVertices { rhs.m_iNumVertices }
	, m_iVertexStride { rhs.m_iVertexStride }
	, m_iNumIndices { rhs.m_iNumIndices }
	, m_iIndexStride { rhs.m_iIndexStride }
	, m_ePrimitive { rhs.m_ePrimitive }
	, m_eIndexFormat { rhs.m_eIndexFormat }

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

HRESULT CVIBuffer::Bind_Resources()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
	};

	_uint VertexStrides[] = {
		m_iVertexStride,
	};

	_uint Offsets[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, VertexStrides, Offsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitive);

	/*
	INPUT_ELEMENT_DESC를 생성하고, Layout 객체를 생성하는 과정.
	*/
	D3D11_INPUT_ELEMENT_DESC Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	ID3D11InputLayout* pInputLayout = { nullptr };
	//m_pDevice->CreateInputLayout(
	//	Elements/*내가 그릴려고하는 정점의 멤버구성정보*/,
	//	2/*내가 그릴려고하는 정점의 멤버변수갯수*/,
	//	쉐이더에서 받아줄수 있는 정점의 정보, 
	//	쉐이더에서 받아줄수 있는 정점의 멤버변수갯수, 
	//	);

	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}


void CVIBuffer::Free()
{
	__super::Free();

	if (false == m_isCloned)
		Safe_Delete_Array(m_pVertexPositions);

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
