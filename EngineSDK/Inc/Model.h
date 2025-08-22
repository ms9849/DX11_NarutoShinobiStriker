#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

public:
	virtual HRESULT Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Bind_Material(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iTextureIndex);
	HRESULT Render(_uint iMeshIndex);
private:

	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer			m_Importer;

private:
	_uint					m_iNumMeshes = {};
	vector<class CMesh*>	m_Meshes;

	_uint					m_iNumMaterials = {};
	vector<class CMaterial*> m_Materials;

	MODEL					m_eType = {};
	_float4x4				m_PreTransformMatrix = {};
private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END
