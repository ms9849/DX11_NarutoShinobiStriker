#pragma once

#include "Component.h"

/*
모델의 정보, 메쉬의 정보, 머테리얼의 정보 전부 저장해야 함.

모델은 메쉬의 갯수 / 메쉬의 정보를 저장해야 한다.
모델은 머테리얼의 갯수 / 머테리얼의 정보를 저장해야 한다.

갯수는 일반 변수로 저장.

갯수는 유동적이므로 갯수에 따른 정보들은 포인터로 저장.

	
	메쉬에서 저장할 정보
	1. 머테리얼 인덱스 ( mMaterialIndex )
	2. 정점 갯수 ( mNumVertices )
	4. 표면 갯수 ( mNumFaces )
	7. vtxmesh의 실제 정보들 ( VTXMESH / mVertices, mNormals, mTangents, mTextureCoords  텍스쿠드  4가지 )
	8. 인덱스 정보 (pIndices, mFaces->mIndices )

	머테리얼에서 저장할 정보
	1. MAX_PATH 까지의 SRV 갯수, 
	2. MAXP_PATH 까지의 SRV 경로.
	-> 27개로 고정시켜도 되지만.. 유동적으로 가능하게끔 하고 싶음.
	저장할때 시간이 좀 걸리더라도 N개만큼 공간 할당해서 세팅해줄 것.
*/

/*
함수 자체는 존재하지만 저장하는 함수는 IMGUI_Manager에서 실행해줄 것
*/
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
	HRESULT Save_Model_ToBinary();

public:
	virtual HRESULT Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize_Prototype(MODEL eType, const _tchar* pBinaryFilePath, _fmatrix PreTransformMatrix);

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
	_bool					m_isBinary = { false };
private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _tchar* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END
