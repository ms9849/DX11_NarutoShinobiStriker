#include "Model.h"

#include "GameInstance.h"
#include "Bone.h"
#include "Mesh.h"
#include "Shader.h"
#include "Material.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{
}

CModel::CModel(const CModel& Prototype)
	: CComponent{ Prototype }
	, m_eType { Prototype.m_eType }
	, m_iNumMeshes { Prototype.m_iNumMeshes }
	, m_Meshes { Prototype.m_Meshes }
	, m_iNumMaterials { Prototype.m_iNumMaterials }
	, m_Materials { Prototype.m_Materials }
	, m_PreTransformMatrix { Prototype.m_PreTransformMatrix }
	, m_MeshNames { Prototype.m_MeshNames }
	, m_Bones{ Prototype.m_Bones }
{
	for (auto& pBone : m_Bones)
		Safe_AddRef(pBone);

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& pMaterial : m_Materials)
		Safe_AddRef(pMaterial);
}

HRESULT CModel::Load_Model_FromBinary(const _tchar* pBinaryFilePath)
{
	/* ../Bin/Resources/Models/Binary/Fiona.bin */
	_char szModelFilePath[MAX_PATH];

	WideCharToMultiByte(CP_ACP, 0, pBinaryFilePath, (_int)_tcslen(pBinaryFilePath),
		szModelFilePath, MAX_PATH, NULL, NULL);

	_char szModelPath[MAX_PATH] = {};
	_tchar szPerefectModelName[MAX_PATH] = {};

	HANDLE hHandle = CreateFile(pBinaryFilePath,
		GENERIC_READ,  // 파일 용도(GENERIC_WRITE : 쓰기(저장), GENERIC_READ : 읽기(불러오기))
		NULL,			// 공유 방식(NULL인 경우 공유하지 않음)
		NULL,			// 보안 설정(NULL인 경우 기본값으로 설정)
		OPEN_EXISTING,	// 생성 방식(CREATE_ALWAYS : 쓰기 전용, OPEN_EXISTING : 읽기 전용)
		FILE_ATTRIBUTE_NORMAL, // 파일 속성(숨김, 읽기 전용 파일 등) : 아무런 속성이 없는 일반 형식
		NULL);	// 생성될 파일의 속성을 제공할 템플릿 파일(안쓸것이기 때문에 NULL)

	if (hHandle == INVALID_HANDLE_VALUE)
	{
		return E_FAIL;
	}

	DWORD	dwByte(0);

	/* 본 갯수 로딩 */
	if (false == ReadFile(hHandle, &m_iNumBones, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;
	/* 본 정보 로딩 */
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		CBone* pBone = CBone::Create(hHandle, &dwByte);
		m_Bones.push_back(pBone);
	}

	/* 머테리얼 갯수 로딩. */
	if (false == ReadFile(hHandle, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;
	/* 머테리얼 정보 로딩 */
	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, hHandle, &dwByte, pBinaryFilePath);
		m_Materials.push_back(pMaterial);
	}

	/* 메쉬 갯수 로딩 */
	if (false == ReadFile(hHandle, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;
	/* 메쉬 이름 로딩 */
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, this, hHandle, &dwByte, XMLoadFloat4x4(&m_PreTransformMatrix));
		m_Meshes.push_back(pMesh);
	}

	CloseHandle(hHandle);

	return S_OK;
}

HRESULT CModel::Save_Model_ToBinary(const _char* pModelSavePath)
{
	if (true == m_isBinary)
		return S_OK;

	_char szModelPath[MAX_PATH] = {};
	_tchar szPerefectModelName[MAX_PATH] = {};
	
	/* ../Bin/Resources/ */

	strcpy_s(szModelPath, pModelSavePath);
	
	/* ../Bin/Resources/Fiona.bin */
	strcat_s(szModelPath, m_szModelName);
	strcat_s(szModelPath, ".Bin");
	/*  char to tchar */
	MultiByteToWideChar(CP_ACP, 0, szModelPath, (_int)strlen(szModelPath),
		szPerefectModelName, MAX_PATH);

	HANDLE hHandle = CreateFile(szPerefectModelName, 
								GENERIC_WRITE,  // 파일 용도(GENERIC_WRITE : 쓰기(저장), GENERIC_READ : 읽기(불러오기))
								NULL,			// 공유 방식(NULL인 경우 공유하지 않음)
								NULL,			// 보안 설정(NULL인 경우 기본값으로 설정)
								CREATE_ALWAYS,	// 생성 방식(CREATE_ALWAYS : 쓰기 전용, OPEN_EXISTING : 읽기 전용)
								FILE_ATTRIBUTE_NORMAL, // 파일 속성(숨김, 읽기 전용 파일 등) : 아무런 속성이 없는 일반 형식
								NULL);	// 생성될 파일의 속성을 제공할 템플릿 파일(안쓸것이기 때문에 NULL)

	if (hHandle == INVALID_HANDLE_VALUE)
		return E_FAIL;

	DWORD	dwByte(0);

	/* 본 갯수 저장 */
	WriteFile(hHandle, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < m_iNumBones; ++i)
		if(FAILED(m_Bones[i]->Save_Bone_ToBinary(hHandle, &dwByte)))
			return E_FAIL;

	/* 머테리얼 갯수 저장 */
	WriteFile(hHandle, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);
	/* 머테리얼 정보 저장 */
	for (_uint i = 0; i < m_iNumMaterials; ++i)
		if(FAILED(m_Materials[i]->Save_Material_ToBinary(hHandle, &dwByte, m_pAIScene->mMaterials[i])))
			return E_FAIL;

	/* 메쉬 갯수 저장 */
	WriteFile(hHandle, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);
	/* 메쉬 이름 저장 (메쉬 문자열 크기, 문자열 순). */
	for (_uint i = 0; i < m_iNumMeshes; ++i)
		if(FAILED(m_Meshes[i]->Save_Mesh_ToBinary(hHandle, &dwByte, m_pAIScene->mMeshes[i])))
			return E_FAIL;

	CloseHandle(hHandle);

	return S_OK;
}

_wstring CModel::Get_MeshName(_uint iIdx) const
{
	return m_Meshes[iIdx]->Get_Name();
}

_int CModel::Get_BoneIndex(const _char* pBoneName) const
{
	_int	iBoneIndex = {};

	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool {
		if (true == pBone->Compare_Name(pBoneName))
			return true;

		++iBoneIndex;

		return false;
	});
	
	if (iter == m_Bones.end())
		return -1;

	return iBoneIndex;
}

HRESULT CModel::Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	_uint			iFlag = {};

	//aiProcessPreset_TargetRealtime_Fast
	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (MODEL::NONANIM == eType)
		iFlag |= aiProcess_PreTransformVertices;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_eType = eType;
	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	_char szFileName[MAX_PATH];
	_splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

	/* Fiona  */
	/* 확장자는 버린다. */
	strcpy_s(m_szModelName, szFileName);

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
		return E_FAIL;

	m_iNumBones = (_uint)m_Bones.size();

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	return S_OK;
}

// 바이너리로 로딩하는 함수
HRESULT CModel::Initialize_Prototype(MODEL eType, const _tchar* pBinaryFilePath, _fmatrix PreTransformMatrix)
{
	m_eType = eType;
	m_isBinary = true;
	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	if(FAILED(Load_Model_FromBinary(pBinaryFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Bind_BoneMatrices(_uint iMeshIndex, CShader* pShader, const _char* pConstantName)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(m_Bones, pShader, pConstantName);
}

HRESULT CModel::Bind_Material(_uint iMeshIndex, CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iTextureIndex)
{
	/* 머테리얼과 메시 양쪽 다 접근하는 함수. 헷갈릴 수 있으니 순서를 잘 봐둘 것 *
	/*
	1. MeshIndex 번째의 Mesh의 "MaterialIndex"를 가져온다.
	2. 해당 Material Index에 해당하는 Material에 접근하여 Bind_SRV를 수행한다.

		pShader->Bind_SRV(pConstantName, m_SRVs[eType][iTextureIndex]);

	셰이더를 넘겨준 뒤, 어떤 재질 타입 (Diffuse Ambient 등 )의 몇번째 텍스쳐를 바인딩 할 것인지 결정한다.
	*/

	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex]->Bind_SRV(pShader, pConstantName, eType, iTextureIndex);
}

void CModel::Play_Animation(_float fTimeDelta)
{
	/* 내가 재생하고자하는 애니메이션(공격모션)이 이용하고 있는 뼈들의 상태 변환정보(TransformationMatrix)를 갱신해준다.*/

	/* 모든 뼈를 순회하면서 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Resources();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, this, m_pAIScene->mMeshes[i], XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_MeshNames.push_back(m_pGameInstance->ToWstring(string(m_pAIScene->mMeshes[i]->mName.C_Str())));
		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, pModelFilePath, m_pAIScene->mMaterials[i]);
		if (nullptr == pMaterial)
			return E_FAIL;

		m_Materials.push_back(pMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
{
	CBone* pBone = CBone::Create(pAINode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	_int	iParent = (_uint)m_Bones.size() - 1;

	for (size_t i = 0; i < pAINode->mNumChildren; i++)
	{
		/* 재귀 형태로 굴러가게 된다. (계층 구조 탐색을 위함) */
		Ready_Bones(pAINode->mChildren[i], iParent);
	}


	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix)))
	{
		MSG_BOX("Create Failed : CModel (By Assimp)");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _tchar* pBinaryFilePath, _fmatrix PreTransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pBinaryFilePath, PreTransformMatrix)))
	{
		MSG_BOX("Create Failed : CModel (By Binary)");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	for (auto& pMaterial : m_Materials)
		Safe_Release(pMaterial);
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();
	
	m_Importer.FreeScene();
}
