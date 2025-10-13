#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CEffectMesh : public CVIBuffer
{
private:
	CEffectMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectMesh(const CEffectMesh& rhs);
	virtual ~CEffectMesh() = default;

public:
	_wstring Get_Name() const;

public:
	virtual HRESULT Initialize_Prototype(MODEL eType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize_Prototype(MODEL eType, HANDLE hHandle, DWORD* dwByte, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Save_Mesh_ToBinary(HANDLE hHandle, DWORD* dwByte, const aiMesh* pAIMesh) const;
	HRESULT Load_Mesh_FromBinary(HANDLE hHandle, DWORD* dwByte, MODEL eType);

private:
	_char						m_szName[MESH_MAX] = {};

private:
	HRESULT Ready_VertexBuffer_For_NonAnim_Assimp(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_VertexBuffer_For_NonAnim_Binary(HANDLE hHandle, DWORD* dwByte);

public:
	static CEffectMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	static CEffectMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, HANDLE hHandle, DWORD* dwByte, _fmatrix PreTransformMatrix = XMMatrixIdentity());

	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
