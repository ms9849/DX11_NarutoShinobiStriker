#include "MapConverter.h"

#include "GameInstance.h"
#include "Props.h"

CMapConverter::CMapConverter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMapConverter::Initialize()
{
	return S_OK;
}

/*
1. 프롭 갯수
2. 프롭 정보
3. 맵 메시 (1개)
4. 오브젝트 갯수 (게이트 나무 + @?)
5. 오브젝트 정보
*/
HRESULT CMapConverter::Export_MapFiles(const _tchar* pMapName, const _tchar* pFilePath, LEVEL eLevelID)
{
	_tchar pMapPath[MAX_PATH] = {};

	_tcscpy_s(pMapPath, MAX_PATH, pFilePath);
	_tcscat_s(pMapPath, MAX_PATH, pMapName);

	HANDLE hHandle = CreateFile(pMapPath,
		GENERIC_WRITE,  // 파일 용도(GENERIC_WRITE : 쓰기(저장), GENERIC_READ : 읽기(불러오기))
		NULL,			// 공유 방식(NULL인 경우 공유하지 않음)
		NULL,			// 보안 설정(NULL인 경우 기본값으로 설정)
		CREATE_ALWAYS,	// 생성 방식(CREATE_ALWAYS : 쓰기 전용, OPEN_EXISTING : 읽기 전용)
		FILE_ATTRIBUTE_NORMAL, // 파일 속성(숨김, 읽기 전용 파일 등) : 아무런 속성이 없는 일반 형식
		NULL);	// 생성될 파일의 속성을 제공할 템플릿 파일(안쓸것이기 때문에 NULL)

	if (hHandle == INVALID_HANDLE_VALUE)
		return E_FAIL;

	DWORD dwByte = {};

	_uint iPropCount = m_pGameInstance->Get_LayerSize(ENUM_CLASS(eLevelID), TEXT("Layer_Props"));

	/* 프롭 갯수 저장 */
	if (false == (WriteFile(hHandle, &iPropCount, sizeof(_uint), &dwByte, nullptr)))
		return E_FAIL;

	/* 프롭 정보 저장 */
	for (_uint i = 0; i < iPropCount; ++i)
	{
		CProps* pProps = static_cast<CProps*>(m_pGameInstance->Get_GameObject(ENUM_CLASS(eLevelID), TEXT("Layer_Props"), i));
		CTransform* pTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(eLevelID), TEXT("Layer_Props"), Engine::g_strTransformTag, i));

		_uint iObjectID = pProps->Get_ObjectID();
		_uint iMeshNum = pProps->Get_MeshIdx();
		_uint iShaderPassIdx = pProps->Get_ShaderPassIdx();
		_float4x4 WorldMatrix = *(pTransform->Get_WorldMatrixPtr());

		if (false == (WriteFile(hHandle, &iObjectID, sizeof(_uint), &dwByte, nullptr)))
			return E_FAIL;

		if (false == (WriteFile(hHandle, &iMeshNum, sizeof(_uint), &dwByte, nullptr)))
			return E_FAIL;

		if (false == (WriteFile(hHandle, &iShaderPassIdx, sizeof(_uint), &dwByte, nullptr)))
			return E_FAIL;

		if (false == (WriteFile(hHandle, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr)))
			return E_FAIL;
	}


	/* 맵 객체 (메시) 갯수 저장 */
	_uint iMapCount = m_pGameInstance->Get_LayerSize(ENUM_CLASS(eLevelID), TEXT("Layer_Map"));
	if (false == (WriteFile(hHandle, &iMapCount, sizeof(_uint), &dwByte, nullptr)))
		return E_FAIL;

	/* 맵 객체 (메시) 정보 저장 */
	for (_uint i = 0; i < iMapCount; ++i)
	{
		CGameObject* pGameObject = m_pGameInstance->Get_GameObject(ENUM_CLASS(eLevelID), TEXT("Layer_Map"), i);
		CTransform* pTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(eLevelID), TEXT("Layer_Props"), Engine::g_strTransformTag, i));

		_uint iObjectID = pGameObject->Get_ObjectID();
		_float4x4 WorldMatrix = *(pTransform->Get_WorldMatrixPtr());

		if (false == (WriteFile(hHandle, &iObjectID, sizeof(_uint), &dwByte, nullptr)))
			return E_FAIL;

		if (false == (WriteFile(hHandle, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr)))
			return E_FAIL;
	}

	/*
	오브젝트는 보류. 시간 남으면 할 것
	*/
	CloseHandle(hHandle);
	return S_OK;
}

HRESULT CMapConverter::Import_MapFiles(const _tchar* pFilePath, LEVEL eLevelID, LEVEL ePrototypeLevelID)
{
	HANDLE hHandle = CreateFile(pFilePath,
		GENERIC_READ,  // 파일 용도(GENERIC_WRITE : 쓰기(저장), GENERIC_READ : 읽기(불러오기))
		NULL,			// 공유 방식(NULL인 경우 공유하지 않음)
		NULL,			// 보안 설정(NULL인 경우 기본값으로 설정)
		OPEN_EXISTING,	// 생성 방식(CREATE_ALWAYS : 쓰기 전용, OPEN_EXISTING : 읽기 전용)
		FILE_ATTRIBUTE_NORMAL, // 파일 속성(숨김, 읽기 전용 파일 등) : 아무런 속성이 없는 일반 형식
		NULL);	// 생성될 파일의 속성을 제공할 템플릿 파일(안쓸것이기 때문에 NULL)

	if (hHandle == INVALID_HANDLE_VALUE)
		return E_FAIL;

	DWORD dwByte = {};

	_uint iPropCount = {};
	/* 프롭 갯수 로드 */
	if (false == (ReadFile(hHandle, &iPropCount, sizeof(_uint), &dwByte, nullptr)))
		return E_FAIL;

	/* 프롭 정보 로드 */
	for (_uint i = 0; i < iPropCount; ++i)
	{
		_uint iObjectID, iMeshNum, iShaderPassIdx;
		_float4x4 WorldMatrix;

		if (false == (ReadFile(hHandle, &iObjectID, sizeof(_uint), &dwByte, nullptr)))
			return E_FAIL;

		if (false == (ReadFile(hHandle, &iMeshNum, sizeof(_uint), &dwByte, nullptr)))
			return E_FAIL;

		if (false == (ReadFile(hHandle, &iShaderPassIdx, sizeof(_uint), &dwByte, nullptr)))
			return E_FAIL;

		if (false == (ReadFile(hHandle, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr)))
			return E_FAIL;

		CProps::PROP_DESC Desc;
		Desc.iMeshIdx = iMeshNum;
		Desc.iShaderPassIdx = iShaderPassIdx;

		m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ePrototypeLevelID), TEXT("Prototype_GameObject_Props"), ENUM_CLASS(eLevelID), TEXT("Layer_Props"), &Desc);
		CTransform* pTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(eLevelID), TEXT("Layer_Props"), Engine::g_strTransformTag, i));
		
		//월드 매트릭스 세팅
		for (_uint j = 0; j < 4; ++j) 
		{
			pTransform->Set_State(static_cast<STATE>(j), XMLoadFloat4((_float4 *)(&WorldMatrix.m[j][0])));
		}
	}

	/* 맵 객체 (메시) 갯수 로드 */
	_uint iMapCount;
	if (false == (ReadFile(hHandle, &iMapCount, sizeof(_uint), &dwByte, nullptr)))
		return E_FAIL;

	/* 맵 객체 (메시) 정보 로드 */
	for (_uint i = 0; i < iMapCount; ++i)
	{
		_uint iObjectID = {};
		_float4x4 WorldMatrix = {};

		if (false == (ReadFile(hHandle, &iObjectID, sizeof(_uint), &dwByte, nullptr)))
			return E_FAIL;

		if (false == (ReadFile(hHandle, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr)))
			return E_FAIL;
		
		/* 튜토맵이냐 / 나뭇잎 마을이냐. 일단은 나뭇잎 마을만 처리해둘 것 */
		if (iObjectID == ENUM_CLASS(Client::OBJECTID::KONOHA_VILLAGE))
		{
			m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ePrototypeLevelID), TEXT("Prototype_GameObject_KonohaVillage"), ENUM_CLASS(eLevelID), TEXT("Layer_Map"));
		}
		else {}

		//월드 매트릭스 세팅
		CTransform* pTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(eLevelID), TEXT("Layer_Map"), Engine::g_strTransformTag, i));

		for (_uint j = 0; j < 4; ++j)
		{
			pTransform->Set_State(static_cast<STATE>(j), XMLoadFloat4((_float4*)(&WorldMatrix.m[j][0])));
		}
	}
	CloseHandle(hHandle);

	return S_OK;
}

CMapConverter* CMapConverter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapConverter* pInstance = new CMapConverter(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Create Failed : Converter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapConverter::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
