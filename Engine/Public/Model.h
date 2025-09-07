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

	/* 애니메이션이 얼마나 진행됐는지 0 ~ 1 사이 값으로 반환 */
	_float Get_CurAnimProgress();
	void   Set_CurAnimProgress(_float fProgress);

	_wstring Get_MeshName(_uint iIdx) const;

	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;
	_int Get_BoneIndex(const _char* pBoneName) const;

public:
	void Set_AnimIndex(const _char* pAnimName, 
		_float fAnimationPlayRate = 1.f, 
		_bool IsBlended = true, 
		_float fBlendRatio = 0.15f,
		_bool IsLoop = false);

public:
	HRESULT Save_Model_ToBinary(const _char* pModelSavePath);
	HRESULT Load_Model_FromBinary(const _tchar* pModelFilePath);

public:
	_bool	Picking_Meshes(_fmatrix WolrdMatrixInverse, _float3* vOut);
	virtual HRESULT Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize_Prototype(MODEL eType, const _tchar* pBinaryFilePath, _fmatrix PreTransformMatrix);

	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Bind_BoneMatrices(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_Material(_uint iMeshIndex, class CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iTextureIndex);
	_bool	Play_Animation(_float fTimeDelta);
	HRESULT Render(_uint iMeshIndex);

private:

	const aiScene*			m_pAIScene = { nullptr };
	Assimp::Importer		m_Importer;

private:
	_uint					m_iNumMeshes = {};

	vector<_wstring>		m_MeshNames = {};
	vector<class CMesh*>	m_Meshes;


	_uint					m_iNumMaterials = {};
	vector<class CMaterial*> m_Materials;

	MODEL					m_eType = {};
	_float4x4				m_PreTransformMatrix = {};
	_bool					m_isBinary = { false };
	_char					m_szModelName[MAX_PATH];

	_uint					m_iNumBones;
	vector<class CBone*>	m_Bones;

	/* 
	현재 애니메이션 인덱스. 
	CurrentAnimIndex가 있다면 PreAnimIndex를 체크해서 
	보간을 수행해줘도 되지 않을까?
	*/	
	_float							m_fAnimationPlayRate = { 1.0f };
	_string							m_strCurrentAnimName;
	_uint							m_iNumAnimations = {};
	_bool							m_isAnimFinished = {};
	map<_string, class CAnimation*>	m_Animations;

	_float						m_fCurTrackPosition = { 0 };
	_float						m_fPreTrackPosition = { 0 };
	map<_int , KEYFRAME>		m_PreAnimKeyFrames = {};
	_bool						m_bAnimationBlending = { false };
	_float						m_fBlendRatio = { 0.15f };

private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	HRESULT Ready_Animations();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _tchar* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END
