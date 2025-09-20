#include "Camera_Manager.h"

#include "GameInstance.h"

#include "Camera.h"

CCamera_Manager::CCamera_Manager()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

/*
컬링 배우면 나중에 컬링으로 대체해야 함.
*/
void CCamera_Manager::SetUp_Target()
{

}

HRESULT CCamera_Manager::Add_TargetTransform(CTransform* pTransformCom)
{
	m_Targets.push_back(pTransformCom);
	Safe_AddRef(pTransformCom);

	return S_OK;
}

HRESULT CCamera_Manager::Add_Camera(LEVEL eLevelID, const _wstring& strCameraTag, CCamera* pCamera)
{
	/* 이미 카메라가 존재하면 FAIL 반환. */
	auto iter = m_Cameras[ENUM_CLASS(eLevelID)].find(strCameraTag);

	if (iter != m_Cameras[ENUM_CLASS(eLevelID)].end())
		return E_FAIL;

	m_Cameras[ENUM_CLASS(eLevelID)].emplace(strCameraTag, pCamera);

	return S_OK;
}

HRESULT CCamera_Manager::Change_Camera(LEVEL eLevelID, const _wstring& strCameraTag)
{
	/* 태그에 해당하는 카메라가 존재하지 않으면 FAIL 반환. */
	auto iter = m_Cameras[ENUM_CLASS(eLevelID)].find(strCameraTag);

	if (m_Cameras[ENUM_CLASS(eLevelID)].end() == iter)
		return E_FAIL;

	/* 같은 태그로 변경하려고 하면 OK만 반환해주게 된다. */
	if (m_strActivatedCameraTag == strCameraTag)
		return S_OK;

	/* 기존 카메라 Set Dead 세팅. 오브젝트 매니저에서 빠져 나오게 된다. */
	if (nullptr != m_pActivatedCamera)
		m_pActivatedCamera->Set_Dead(true);

	static_cast<CGameObject*>(iter->second)->Set_Dead(false);
	static_cast<CCamera*>(iter->second)->OnChange();
	m_pGameInstance->Add_Clone_ToLayer(iter->second, ENUM_CLASS(eLevelID), TEXT("Layer_Camera"));

	/* 활성화중인 카메라 교체 */
	m_strActivatedCameraTag = strCameraTag;
	m_pActivatedCamera = iter->second;
	Safe_AddRef(m_pActivatedCamera);

	return S_OK;
}

void CCamera_Manager::Clear()
{
	/* 카메라 지우기 */
	for (_uint i = 0; i < ENUM_CLASS(LEVEL::END); ++i)
	{
		for (auto& pCamera : m_Cameras[i])
			Safe_Release(pCamera.second);

		m_Cameras[i].clear();
	}

	for (auto& pTransform : m_Targets)
	{
		Safe_Release(pTransform);
		m_Targets.clear();
	}

	m_pActivatedCamera = nullptr;
}

CCamera_Manager* CCamera_Manager::Create()
{
	return new CCamera_Manager();
}

void CCamera_Manager::Free()
{
	__super::Free();

	/* 카메라 지우기 */
	for (_uint i = 0; i < ENUM_CLASS(LEVEL::END); ++i)
	{
		for (auto& pCamera : m_Cameras[i])
			Safe_Release(pCamera.second);

		m_Cameras[i].clear();
	}

	for (auto& pTransform : m_Targets)
	{
		Safe_Release(pTransform);
		m_Targets.clear();
	}

	m_pActivatedCamera = nullptr;

	Safe_Release(m_pGameInstance);

}
