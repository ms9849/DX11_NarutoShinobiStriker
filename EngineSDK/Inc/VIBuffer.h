#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	_uint Get_NumIndices() const { return m_iNumIndices; }
	_uint Get_NumVertices() const { return m_iNumVertices; }
	_float3* Get_VertexPositions() const { return m_pVertexPositions; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Picking(_fmatrix WolrdMatrixInverse, _float3* pOut);

public:
	virtual HRESULT	Bind_Resources();
	virtual HRESULT Render();

protected:
	ID3D11Buffer*	m_pVB = { nullptr };
	ID3D11Buffer*	m_pIB = { nullptr };

	_uint			m_iNumVertexBuffers = {};
	_float3*		m_pVertexPositions = {};

	_uint			m_iNumVertices = {};
	_uint			m_iVertexStride = {};

	_uint			m_iNumIndices = {};
	_uint			m_iIndexStride = {};
	DXGI_FORMAT		m_eIndexFormat = {};
	/* Topology -> 위상이란 뜻 */
	D3D11_PRIMITIVE_TOPOLOGY m_ePrimitive = {};

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free();
};

NS_END
