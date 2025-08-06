#include "Camera_Manager.h"

#include "Camera.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CCamera_Manager);

CCamera_Manager::CCamera_Manager()
    : m_pGameInstance { CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CCamera_Manager::Initialize_CameraManager()
{
    return S_OK;
}

void CCamera_Manager::Update(_float fTimeDelta)
{
    //카메라 가져와서 싸악 세팅.. 일단 이 부분은 수업 듣고 할 것.
    //m_pGameInstance->Set_CameraWorldMatrix();
}

HRESULT CCamera_Manager::Add_Camera(const _wstring& strCameraTag, CCamera* pCamera)
{
    auto iter = m_Cameras.find(strCameraTag);
    /* 이미 같은 태그로 존재한다면, */
    if (iter != m_Cameras.end())
        return E_FAIL;

    m_Cameras.emplace(strCameraTag, pCamera);
    Safe_AddRef(pCamera);

    return S_OK;
}

HRESULT CCamera_Manager::Change_Camera(const _wstring& strCameraTag)
{
    auto iter = m_Cameras.find(strCameraTag);

    if (iter == m_Cameras.end())
        return E_FAIL;

    m_ActivatedCamera = iter->second;

    return S_OK;
}

void CCamera_Manager::Release_CameraManager()
{
    DestroyInstance();

    Safe_Release(m_pGameInstance);
}

void CCamera_Manager::Free()
{
    __super::Free();

    for (auto& iter : m_Cameras)
    {
        Safe_Release(iter.second);
    }
    
    m_Cameras.clear();
}
