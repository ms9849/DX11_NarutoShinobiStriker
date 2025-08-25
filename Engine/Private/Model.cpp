#include "Model.h"

#include "GameInstance.h"
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
{
	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& pMaterial : m_Materials)
		Safe_AddRef(pMaterial);
}

HRESULT CModel::Load_NonAnimModel_Assimp(const _char* pModelFilePath)
{
	/*
	요 플래그가 매우 중요하다고 하심.
	방학 끝나고 한번 설명해준다곤 하셨는데..
	*/
	_uint			iFlag = {};

	//aiProcessPreset_TargetRealtime_Fast
	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (MODEL::NONANIM == m_eType)
		iFlag |= aiProcess_PreTransformVertices;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);

	if (nullptr == m_pAIScene)
		return E_FAIL;

	_char szFileName[MAX_PATH];
	_splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

	/* Fiona  */
	/* 확장자는 버린다. */
	strcpy_s(m_szModelName, szFileName);

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;


	return S_OK;
}

HRESULT CModel::Load_NonAnimModel_Binary(const _tchar* pBinaryFilePath)
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

#pragma region LOAD_MATERIAL
	/* 머테리얼 갯수 로딩. */
	if (false == ReadFile(hHandle, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;

	/* 머테리얼 정보 로딩 */
	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		IMPORT_MATERIAL_DESC* ImportMaterialDesc = new IMPORT_MATERIAL_DESC();

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			if (false == ReadFile(hHandle, &ImportMaterialDesc->iNumSRVs[j], sizeof(_uint), &dwByte, nullptr))
				return E_FAIL;

			/* 보통은 1개만 있으니까.. */
			for (_uint k = 0; k < ImportMaterialDesc->iNumSRVs[j]; ++k)
			{
				if (false == ReadFile(hHandle, ImportMaterialDesc->szTexturePath[j], MAX_PATH, &dwByte, nullptr))
					return E_FAIL;
			}
		}

		CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, pBinaryFilePath, *ImportMaterialDesc);
		if (nullptr == pMaterial)
			return E_FAIL;

		m_Materials.push_back(pMaterial);

		Safe_Delete(ImportMaterialDesc);
	}
#pragma endregion

#pragma region LOAD_MESH
	/* 메쉬 갯수 로딩 */
	if (false == ReadFile(hHandle, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;

	///* 메쉬 이름 로딩 */
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		_int iLength;
		_wstring strMeshName;

		if (false == ReadFile(hHandle, &iLength, sizeof(_int), &dwByte, nullptr))
			return E_FAIL;

		strMeshName.resize(iLength);

		if (false == ReadFile(hHandle, (void *)strMeshName.data(), sizeof(_tchar) * iLength, &dwByte, nullptr))
			return E_FAIL;

		m_MeshNames.push_back(strMeshName);
	}

	IMPORT_MESH_DESC ImportMeshDesc;

	/* 메쉬 정보 로딩 */
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		/* iMaterialIndex, iNumFaces, iNumVertices */
		if (false == ReadFile(hHandle, &ImportMeshDesc, sizeof(_uint) * 3, &dwByte, nullptr))
			return E_FAIL;

		ImportMeshDesc.pVertices = new VTXMESH[ImportMeshDesc.iNumVertices];
		ImportMeshDesc.pIndices = new _uint[ImportMeshDesc.iNumFaces * 3];

		/* 정점 정보 로딩 */
		for (_uint j = 0; j < ImportMeshDesc.iNumVertices; ++j)
		{
			if (false == ReadFile(hHandle, &ImportMeshDesc.pVertices[j], sizeof(VTXMESH), &dwByte, nullptr))
				return E_FAIL;
		}

		_uint iNumIndices = { 0 };

		/* 인덱스 정보 로딩 */
		for (_uint j = 0; j < ImportMeshDesc.iNumFaces; ++j)
		{
			if (false == ReadFile(hHandle, &ImportMeshDesc.pIndices[iNumIndices], sizeof(_uint) * 3, &dwByte, nullptr))
				return E_FAIL;

			iNumIndices += 3;
		}

		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, ImportMeshDesc, XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);

		Safe_Delete_Array(ImportMeshDesc.pIndices);
		Safe_Delete_Array(ImportMeshDesc.pVertices);
	}

#pragma endregion
	CloseHandle(hHandle);

	return S_OK;
}

HRESULT CModel::Save_NonAnimModel_ToBinary(const _char* pModelSavePath)
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
	{
		return E_FAIL;
	}

	DWORD	dwByte(0);

#pragma region SAVE_MATERIAL
	/* 머테리얼 갯수 저장 */
	WriteFile(hHandle, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);

	/* 머테리얼 정보 저장 */
	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];
		EXPORT_MATERIAL_DESC MaterialDesc;

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			/* 텍스쳐 갯수 저장 */
			MaterialDesc.iNumSRVs[j] = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(j));
			WriteFile(hHandle, &MaterialDesc.iNumSRVs[j], sizeof(_uint), &dwByte, nullptr);
			/* 텍스쳐 경로 저장 */
			for (_uint k = 0; k < MaterialDesc.iNumSRVs[j]; ++k)
			{
				aiString strTexturePath;

				_char szDrive[MAX_PATH] = {};
				_char szDir[MAX_PATH] = {};
				_char szFileName[MAX_PATH] = {};
				_char szEXT[MAX_PATH] = {};

				/* 만약 텍스쳐가 존재하지 않는다면 continue */
				pAIMaterial->GetTexture(static_cast<aiTextureType>(j), k, &strTexturePath);

				_char szTextureFilePath[MAX_PATH] = {};
				/* 드라이브 경로 / 파일 경로 / 파일 이름 / 파일 확장자 4개로 나뉘는걸 유의할 것 */

				/* 모델을 저장할 경로의 드라이브, 파일 경로를 가져온다. */
				_splitpath_s(pModelSavePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

				/* 텍스쳐 파일 경로로부터 이름, 확장자를 가져온다. */
				_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

				strcpy_s(szTextureFilePath, szDrive);
				strcat_s(szTextureFilePath, szDir);
				strcat_s(szTextureFilePath, szFileName);
				strcat_s(szTextureFilePath, szEXT);

				WriteFile(hHandle, szTextureFilePath, MAX_PATH, &dwByte, nullptr);
			}

		}
	}
#pragma endregion

#pragma region SAVE_MESH
	/* 메쉬 갯수 저장 */
	WriteFile(hHandle, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	/* 메쉬 이름 저장 (메쉬 문자열 크기, 문자열 순). */

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		const _tchar* pString = m_MeshNames[i].c_str();
		_int iSize = m_MeshNames[i].length();

		WriteFile(hHandle, &iSize, sizeof(_int), &dwByte, nullptr);
		WriteFile(hHandle, pString, sizeof(_tchar) * iSize, &dwByte, nullptr);
	}

	/* 메쉬 정보 저장 */
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		aiMesh* pAIMesh = m_pAIScene->mMeshes[i];
		EXPORT_MESH_DESC MeshDesc;

		MeshDesc.iMaterialIndex = pAIMesh->mMaterialIndex;
		MeshDesc.iNumFaces = pAIMesh->mNumFaces;
		MeshDesc.iNumVertices = pAIMesh->mNumVertices;

		// 12바이트 저장. 이후에는 동적으로 pIndices, pVertices 저장해줄 예정
		WriteFile(hHandle, &MeshDesc, sizeof(_uint) * 3, &dwByte, nullptr);

		/* 정점 정보 저장 */
		for (_uint j = 0; j < MeshDesc.iNumVertices; ++j)
		{
			/* 인덱스 갯수는 삼각형의 갯수 * 3개 */
			VTXMESH VtxMesh;

			memcpy(&VtxMesh.vPosition, &pAIMesh->mVertices[j], sizeof(_float3));
			memcpy(&VtxMesh.vNormal, &pAIMesh->mNormals[j], sizeof(_float3));
			memcpy(&VtxMesh.vTangent, &pAIMesh->mTangents[j], sizeof(_float3));
			memcpy(&VtxMesh.vTexcoord, &pAIMesh->mTextureCoords[0][j], sizeof(_float2));

			WriteFile(hHandle, &VtxMesh, sizeof(VtxMesh), &dwByte, nullptr);
		}

		_uint Indices[3];
		
		/* 인덱스 정보 저장 */
		for (_uint j = 0; j < pAIMesh->mNumFaces; ++j)
		{
			Indices[0] = pAIMesh->mFaces[j].mIndices[0];
			Indices[1] = pAIMesh->mFaces[j].mIndices[1];
			Indices[2] = pAIMesh->mFaces[j].mIndices[2];

			WriteFile(hHandle, &Indices, sizeof(_uint) * 3, &dwByte, nullptr);
		}
	}
#pragma endregion
	CloseHandle(hHandle);

	return S_OK;
}

HRESULT CModel::Load_AnimModel_Assimp()
{
	return S_OK;
}

HRESULT CModel::Load_AnimModel_Binary()
{
	return S_OK;
}

HRESULT CModel::Save_AnimModel_ToBinary(const _char* pModelSavePath)
{
	return S_OK;
}

/* 바이너리로 저장하는 함수 */
HRESULT CModel::Save_Model_ToBinary(const _char* pModelSavePath)
{
	if (MODEL::NONANIM == m_eType)
		Save_NonAnimModel_ToBinary(pModelSavePath);


	else if (MODEL::ANIM == m_eType)
		Save_AnimModel_ToBinary(pModelSavePath);

	return S_OK;
}

HRESULT CModel::Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	m_eType = eType;
	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
	
	if (MODEL::NONANIM == m_eType)
		Load_NonAnimModel_Assimp(pModelFilePath);

	/* 아직 구현되지 않음 */
	else if (MODEL::ANIM == m_eType)
		Load_AnimModel_Assimp();

	return S_OK;
}

// 바이너리로 로딩하는 함수
HRESULT CModel::Initialize_Prototype(MODEL eType, const _tchar* pBinaryFilePath, _fmatrix PreTransformMatrix)
{
	m_eType = eType;
	m_isBinary = true;
	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	HRESULT hr;

	if (MODEL::NONANIM == m_eType)
		hr = Load_NonAnimModel_Binary(pBinaryFilePath);

	/* 아직 구현되지 않음 */
	else if (MODEL::ANIM == m_eType)
		hr = Load_AnimModel_Binary();

	return hr;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
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
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_pAIScene->mMeshes[i], XMLoadFloat4x4(&m_PreTransformMatrix));
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

	for (auto& pMaterial : m_Materials)
		Safe_Release(pMaterial);
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();
	
	m_Importer.FreeScene();
}
