#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize_Prototype(const IMPORT_MESH_DESC& Desc, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

private:
	_uint		m_iMaterialIndex;

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const IMPORT_MESH_DESC& Desc, _fmatrix PreTransformMatrix = XMMatrixIdentity());

	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
