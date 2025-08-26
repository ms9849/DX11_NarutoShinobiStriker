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
	virtual HRESULT Initialize_Prototype(MODEL eType, const class CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize_Prototype(MODEL eType, const class CModel* pModel, const IMPORT_MESH_DESC& Desc, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT	Bind_BoneMatrices(const vector<class CBone*>& Bones, class CShader* pShader, const _char* pConstantName);

private:
	_char				m_szName[MAX_PATH] = {};
	_uint				m_iMaterialIndex;
	_uint				m_iNumBones = { };
	vector<_int>		m_BoneIndices;
	_float4x4*			m_pBoneMatrices = { nullptr };
	vector<_float4x4>	m_OffsetMatrices;

private:
	HRESULT Ready_VertexBuffer_For_NonAnim_Assimp(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_Anim_Assimp(const class CModel* pModel, const aiMesh* pAIMesh);

	HRESULT Ready_VertexBuffer_For_NonAnim_Binary(const IMPORT_MESH_DESC& Desc, _fmatrix PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_Anim_Binary(const class CModel* pModel, const IMPORT_MESH_DESC& Desc);
public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const class CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const class CModel* pModel, const IMPORT_MESH_DESC& Desc, _fmatrix PreTransformMatrix = XMMatrixIdentity());

	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
