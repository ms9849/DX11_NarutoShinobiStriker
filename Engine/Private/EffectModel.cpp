#include "EffectModel.h"

#include "Material.h"
#include "EffectMesh.h"
#include "GameInstance.h"

CEffectModel::CEffectModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CEffectModel::CEffectModel(const CEffectModel& Prototype)
	: CComponent{ Prototype }
	, m_eType{ Prototype.m_eType }
	, m_iNumMeshes{ Prototype.m_iNumMeshes }
	, m_MeshNames{ Prototype.m_MeshNames }
	, m_Meshes{ Prototype.m_Meshes }
	, m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
{
}

HRESULT CEffectModel::Save_Model_ToBinary(const _char* pModelSavePath)
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
	MultiByteToWideChar(CP_ACP, 0, szModelPath, (_int)strlen(szModelPath), szPerefectModelName, MAX_PATH);

	DWORD	dwByte(0);
	HANDLE hHandle = CreateFile(szPerefectModelName,
		GENERIC_WRITE,  // 파일 용도(GENERIC_WRITE : 쓰기(저장), GENERIC_READ : 읽기(불러오기))
		NULL,			// 공유 방식(NULL인 경우 공유하지 않음)
		NULL,			// 보안 설정(NULL인 경우 기본값으로 설정)
		CREATE_ALWAYS,	// 생성 방식(CREATE_ALWAYS : 쓰기 전용, OPEN_EXISTING : 읽기 전용)
		FILE_ATTRIBUTE_NORMAL, // 파일 속성(숨김, 읽기 전용 파일 등) : 아무런 속성이 없는 일반 형식
		NULL);	// 생성될 파일의 속성을 제공할 템플릿 파일(안쓸것이기 때문에 NULL)

	if (hHandle == INVALID_HANDLE_VALUE)
		return E_FAIL;

	/* 메쉬 갯수 저장 */
	WriteFile(hHandle, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);
	/* 메쉬 이름 저장 (메쉬 문자열 크기, 문자열 순). */
	for (_uint i = 0; i < m_iNumMeshes; ++i)
		if (FAILED(m_Meshes[i]->Save_Mesh_ToBinary(hHandle, &dwByte, m_pAIScene->mMeshes[i])))
			return E_FAIL;

	CloseHandle(hHandle);

	return S_OK;
}

HRESULT CEffectModel::Load_Model_FromBinary(const _tchar* pBinaryFilePath)
{
	/* ../Bin/Resources/Models/Binary/Fiona.bin */
	_char szModelFilePath[MAX_PATH];

	WideCharToMultiByte(CP_ACP, 0, pBinaryFilePath, (_int)_tcslen(pBinaryFilePath), szModelFilePath, MAX_PATH, NULL, NULL);

	_char szModelPath[MAX_PATH] = {};
	_tchar szPerefectModelName[MAX_PATH] = {};

	DWORD	dwByte(0);
	HANDLE hHandle = CreateFile(pBinaryFilePath,
		GENERIC_READ,  // 파일 용도(GENERIC_WRITE : 쓰기(저장), GENERIC_READ : 읽기(불러오기))
		NULL,			// 공유 방식(NULL인 경우 공유하지 않음)
		NULL,			// 보안 설정(NULL인 경우 기본값으로 설정)
		OPEN_EXISTING,	// 생성 방식(CREATE_ALWAYS : 쓰기 전용, OPEN_EXISTING : 읽기 전용)
		FILE_ATTRIBUTE_NORMAL, // 파일 속성(숨김, 읽기 전용 파일 등) : 아무런 속성이 없는 일반 형식
		NULL);	// 생성될 파일의 속성을 제공할 템플릿 파일(안쓸것이기 때문에 NULL)

	if (hHandle == INVALID_HANDLE_VALUE)
		return E_FAIL;

	/* 메쉬 갯수 로딩 */
	if (false == ReadFile(hHandle, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;
	/* 메쉬 이름 로딩 */
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CEffectMesh* pMesh = CEffectMesh::Create(m_pDevice, m_pContext, m_eType, hHandle, &dwByte, XMLoadFloat4x4(&m_PreTransformMatrix));
		m_Meshes.push_back(pMesh);
	}

	CloseHandle(hHandle);

	return S_OK;
}

HRESULT CEffectModel::Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	_uint			iFlag = {};

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

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectModel::Initialize_Prototype(MODEL eType, const _tchar* pBinaryFilePath, _fmatrix PreTransformMatrix)
{
	m_eType = eType;
	m_isBinary = true;
	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	if (FAILED(Load_Model_FromBinary(pBinaryFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CEffectModel::Render(_uint iMeshIndex)
{
	//여기서 활성화된 메시들만 출력하도록 한다.
	m_Meshes[iMeshIndex]->Bind_Resources();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CEffectModel::Ready_Meshes()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CEffectMesh* pMesh = CEffectMesh::Create(m_pDevice, m_pContext, m_eType, m_pAIScene->mMeshes[i], XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_MeshNames.push_back(m_pGameInstance->ToWstring(string(m_pAIScene->mMeshes[i]->mName.C_Str())));
		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

CEffectModel* CEffectModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	CEffectModel* pInstance = new CEffectModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CEffectModel Create Failed");
	}
	
	return pInstance;
}

CEffectModel* CEffectModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _tchar* pModelFilePath, _fmatrix PreTransformMatrix)
{
	CEffectModel* pInstance = new CEffectModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CEffectModel Create Failed");
	}

	return pInstance;
}

CComponent* CEffectModel::Clone(void* pArg)
{
	CEffectModel* pInstance = new CEffectModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CEffectModel Clone Failed");
	}

	return pInstance;
}

void CEffectModel::Free()
{
	__super::Free();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Meshes.clear();
	m_Importer.FreeScene();
}
