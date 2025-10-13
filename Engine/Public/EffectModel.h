#pragma once

#include "Component.h"

/*
모델의 NonAnim 부분만 잘라다가 쓸 예정.

UV 세팅 & 텍스쳐 교환등이 조금 더 원활할 수 있게 생각해봐야 됨.
*/
NS_BEGIN(Engine)

class ENGINE_DLL CEffectModel final : public CComponent
{
private:
	CEffectModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectModel(const CEffectModel& Prototype);
	virtual ~CEffectModel() = default;

public:
	_uint Get_NumMeshes() const { return m_iNumMeshes; }
	class CEffectMesh* Get_Mesh(_int iIdx) { return m_Meshes[iIdx]; }

public:
	HRESULT Save_Model_ToBinary(const _char* pModelSavePath);
	HRESULT Load_Model_FromBinary(const _tchar* pModelFilePath);

public:
	virtual HRESULT Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize_Prototype(MODEL eType, const _tchar* pBinaryFilePath, _fmatrix PreTransformMatrix);

	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Render(_uint iMeshIndex);

private:

	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer			m_Importer;

private:
	_uint						m_iNumMeshes = {};
	vector<_wstring>			m_MeshNames = {};
	vector<class CEffectMesh*>		m_Meshes;

	MODEL						m_eType = {};
	_float4x4					m_PreTransformMatrix = {};
	_bool						m_isBinary = { false };
	_char						m_szModelName[MAX_PATH];

private:
	HRESULT Ready_Meshes();

public:
	static CEffectModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	static CEffectModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _tchar* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END
