#include "MainCamera.h"

#include "GameManager.h"
#include "GameInstance.h"

CMainCamera::CMainCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CCamera{ pDevice, pContext, ENUM_CLASS(eObjectID) }
    , m_pGameManager{ CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

CMainCamera::CMainCamera(const CMainCamera& rhs)
    : CCamera{ rhs }
    , m_pGameManager{ CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

HRESULT CMainCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMainCamera::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_vCameraPos = _float3{ 0.f, 4.f, -4.f };
    m_pPlayerTransform = static_cast<MAIN_CAMERA_DESC*>(pArg)->pPlayerTransform;
    m_pTransformCom->Set_State(STATE::POSITION, m_pPlayerTransform->Get_State(STATE::POSITION) + XMVectorSet(m_vCameraPos.x, m_vCameraPos.y, m_vCameraPos.z, 0.f));
    Safe_AddRef(m_pPlayerTransform);

    return S_OK;
}

void CMainCamera::Priority_Update(_float fTimeDelta)
{
    if (false == m_IsLockOn)
    {
        m_fTimeAcc += fTimeDelta;

        if (1.0f <= m_fTimeAcc)
        {
            m_IsLockOn = true;
            m_fTimeAcc = 0.f;
        }
    }

    Look_Target(fTimeDelta);
    Chase_Target(fTimeDelta);
    Mouse_Lock();

    __super::Bind_Matrices();
}

void CMainCamera::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CMainCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CMainCamera::Render()
{
    return S_OK;
}

void CMainCamera::OnChange(const _float4x4* pWorldMatrix)
{
    if (nullptr != pWorldMatrix)
        m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(pWorldMatrix));
}

void CMainCamera::Rotate_Point(_float fTimeDelta)
{
    _float fMouseMoveX = (_float)m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::X) / g_iWinSizeX;
    _float fMouseMoveY = (_float)m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::Y) / g_iWinSizeY;

    // 회전할 벡터와 각도
    _vector  StartVector = XMVectorSet(0.f, 2.5f, -3.0f, 0.f);

    m_fRotateX += XMConvertToRadians(fMouseMoveX * 90.f);
    m_fRotateX = XMScalarModAngle(m_fRotateX);

    m_fRotateY += XMConvertToRadians(fMouseMoveY * 45.f);
    m_fRotateY = XMScalarModAngle(m_fRotateY);

    // 라디안 제한 ( -90  ~ +90 + 여기에 캐릭터의 Look 벡터까지. )
    _float fLimit = XMConvertToRadians(30.f);

    if (m_fRotateY > fLimit)
        m_fRotateY = fLimit;

    if (m_fRotateY < -fLimit)
        m_fRotateY = -fLimit;

    _vector		vQuternion = XMQuaternionRotationRollPitchYaw(m_fRotateY, m_fRotateX, 0.f);

    _matrix		RotationMatrix = XMMatrixRotationQuaternion(vQuternion);
    _vector     vCamPos = XMVector3Rotate(StartVector, vQuternion);
    _float      fLength = XMVectorGetX(XMVector3Length(vCamPos));

    m_pTransformCom->Chase_Lerp(m_pPlayerTransform->Get_State(STATE::POSITION) + vCamPos, fTimeDelta * 0.9f, 0.f);
}

void CMainCamera::Mouse_Lock()
{
    // 클라이언트 영역 크기 얻기
    RECT rcClient;
    GetClientRect(g_hWnd, &rcClient);

    _int clientCenterX = (rcClient.right - rcClient.left) / 2;
    _int clientCenterY = (rcClient.bottom - rcClient.top) / 2;

    // 클라이언트 좌표 → 스크린 좌표로 변환
    POINT pt = { clientCenterX, clientCenterY };
    ClientToScreen(g_hWnd, &pt);
    SetCursorPos(pt.x, pt.y);
}

CMainCamera* CMainCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CMainCamera* pInstance = new CMainCamera(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : MainCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMainCamera::Clone(void* pArg)
{
    CMainCamera* pInstance = new CMainCamera(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clones Failed : MainCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMainCamera::Free()
{
    __super::Free();

    Safe_Release(m_pGameManager);
    Safe_Release(m_pPlayerTransform);
}


void CMainCamera::Look_Target(_float fTimeDelta)
{
    CTransform* pTargetTransform = m_pGameManager->Calc_Target(m_pPlayerTransform->Get_State(STATE::POSITION));

    if (nullptr != pTargetTransform)
    {
        _vector vPlayerPos = m_pPlayerTransform->Get_State(STATE::POSITION);
        _vector vPlayerLook = m_pPlayerTransform->Get_State(STATE::LOOK);
        _vector vCameraPos = XMLoadFloat4(m_pGameInstance->Get_CamState(STATE::POSITION));
        _vector vCameraLook = XMVectorSetY(XMLoadFloat4(m_pGameInstance->Get_CamState(STATE::LOOK)), 0.f);

        _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);

        _float fPlayerDot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vPlayerLook), XMVector3Normalize(XMVectorSetY(vTargetPos - vPlayerPos, 0.f))));
        _float fCameraDot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMVectorSetY(vCameraLook, 0.f)), XMVector3Normalize(XMVectorSetY(vTargetPos - vCameraPos, 0.f))));

        if ((fPlayerDot > 0.f) && (fCameraDot > 0.f))
        {
            _vector vLookPos = m_pPlayerTransform->Get_State(STATE::POSITION) + 2.f * XMVector3Normalize(pTargetTransform->Get_State(STATE::POSITION) - m_pPlayerTransform->Get_State(STATE::POSITION)
                + XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::UP)) * 1.f);
            m_pTransformCom->LookAt_Lerp(vLookPos, 0.175f);
        }
        else
        {
            m_pTransformCom->LookAt_Lerp(
                m_pPlayerTransform->Get_State(STATE::POSITION) +
                XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::LOOK)) * 1.f +
                XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::UP)) * 1.f, 0.175f);
        }
    }
    else
    {
        m_pTransformCom->LookAt_Lerp(
            m_pPlayerTransform->Get_State(STATE::POSITION) +
            XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::LOOK)) * 1.f +
            XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::UP)) * 1.f, 0.175f);
    }
}

void CMainCamera::Chase_Target(_float fTimeDelta)
{
    _float fMouseMoveX = (_float)m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::X) / g_iWinSizeX;
    _float fMouseMoveY = (_float)m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::Y) / g_iWinSizeY;

    // 회전할 벡터와 각도
    _vector  StartVector = XMVectorSet(0.f, 2.5f, -3.0f, 0.f);

    m_fRotateX += XMConvertToRadians(fMouseMoveX * 90.f);
    m_fRotateX = XMScalarModAngle(m_fRotateX);

    m_fRotateY += XMConvertToRadians(fMouseMoveY * 45.f);
    m_fRotateY = XMScalarModAngle(m_fRotateY);

    // 라디안 제한 ( -90  ~ +90 + 여기에 캐릭터의 Look 벡터까지. )
    _float fLimit = XMConvertToRadians(15.f);

    if (m_fRotateY > fLimit)
        m_fRotateY = fLimit;

    if (m_fRotateY < -fLimit)
        m_fRotateY = -fLimit;

    _vector		vQuternion = XMQuaternionRotationRollPitchYaw(m_fRotateY, m_fRotateX, 0.f);

    _matrix		RotationMatrix = XMMatrixRotationQuaternion(vQuternion);
    _vector     vCamPos = XMVector3Rotate(StartVector, vQuternion);
    _float      fLength = XMVectorGetX(XMVector3Length(vCamPos));

    m_pTransformCom->Chase_Lerp(m_pPlayerTransform->Get_State(STATE::POSITION) + vCamPos, fTimeDelta * 0.9f, 0.f);
}

//#include "MainCamera.h"
//
//#include "GameManager.h"
//#include "GameInstance.h"
//
//CMainCamera::CMainCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
//    : CCamera { pDevice, pContext, ENUM_CLASS(eObjectID) }
//    , m_pGameManager { CGameManager::GetInstance() }
//{
//    Safe_AddRef(m_pGameManager);
//}
//
//CMainCamera::CMainCamera(const CMainCamera& rhs)
//    : CCamera{ rhs }
//    , m_pGameManager { CGameManager::GetInstance() }
//{
//    Safe_AddRef(m_pGameManager);
//}
//
//HRESULT CMainCamera::Initialize_Prototype()
//{
//    return S_OK;
//}
//
//HRESULT CMainCamera::Initialize(void* pArg)
//{
//    if (FAILED(__super::Initialize(pArg)))
//        return E_FAIL;
//
//    m_vCameraPos = _float3{ 0.f, 4.f, -4.f };
//    m_pPlayerTransform = static_cast<MAIN_CAMERA_DESC*>(pArg)->pPlayerTransform;
//    m_pTransformCom->Set_State(STATE::POSITION, m_pPlayerTransform->Get_State(STATE::POSITION) + XMVectorSet(m_vCameraPos.x, m_vCameraPos.y, m_vCameraPos.z, 0.f));
//    Safe_AddRef(m_pPlayerTransform);
//
//    return S_OK;
//}
//
//void CMainCamera::Priority_Update(_float fTimeDelta)
//{
//    if (false == m_IsLockOn)
//    {
//        m_fTimeAcc += fTimeDelta;
//
//        if (1.0f <= m_fTimeAcc)
//        {
//            m_IsLockOn = true;
//            m_fTimeAcc = 0.f;
//        }
//    }
//
//    Look_Target(fTimeDelta);
//    Chase_Target(fTimeDelta);
//    Mouse_Lock();
//
//    __super::Bind_Matrices();
//}
//
//void CMainCamera::Update(_float fTimeDelta)
//{
//}
//
//void CMainCamera::Late_Update(_float fTimeDelta)
//{
//}
//
//HRESULT CMainCamera::Render()
//{
//    return S_OK;
//}
//
//void CMainCamera::OnChange(const _float4x4* pWorldMatrix)
//{
//    if (nullptr != pWorldMatrix)
//        m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(pWorldMatrix));
//}
//
//void CMainCamera::Rotate_Point(_float fTimeDelta)
//{
//    _float fMouseMoveX = (_float)m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::X) / g_iWinSizeX;
//    _float fMouseMoveY = (_float)m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::Y) / g_iWinSizeY;
//
//    // 회전할 벡터와 각도
//    _vector  StartVector = XMVectorSet(0.f, 2.5f, -3.0f, 0.f);
//
//    m_fRotateX += XMConvertToRadians(fMouseMoveX * 90.f);
//    m_fRotateX = XMScalarModAngle(m_fRotateX);
//
//    m_fRotateY += XMConvertToRadians(fMouseMoveY * 45.f);
//    m_fRotateY = XMScalarModAngle(m_fRotateY);
//
//    // 라디안 제한 ( -90  ~ +90 + 여기에 캐릭터의 Look 벡터까지. )
//    _float fLimit = XMConvertToRadians(15.f);
//
//    if (m_fRotateY > fLimit)
//        m_fRotateY = fLimit;
//
//    if (m_fRotateY < -fLimit)
//        m_fRotateY = -fLimit;
//
//    _vector		vQuternion = XMQuaternionRotationRollPitchYaw(m_fRotateY, m_fRotateX, 0.f);
//
//    _matrix		RotationMatrix = XMMatrixRotationQuaternion(vQuternion);
//    _vector     vCamPos = XMVector3Rotate(StartVector, vQuternion);
//    _float      fLength = XMVectorGetX(XMVector3Length(vCamPos));
//
//    m_pTransformCom->Chase_Lerp(m_pPlayerTransform->Get_State(STATE::POSITION) + vCamPos, fTimeDelta * 0.5f, 0.f);
//}
//
//void CMainCamera::Mouse_Lock()
//{
//    // 클라이언트 영역 크기 얻기
//    RECT rcClient;
//    GetClientRect(g_hWnd, &rcClient);
//
//    _int clientCenterX = (rcClient.right - rcClient.left) / 2;
//    _int clientCenterY = (rcClient.bottom - rcClient.top) / 2;
//
//    // 클라이언트 좌표 → 스크린 좌표로 변환
//    POINT pt = { clientCenterX, clientCenterY };
//    ClientToScreen(g_hWnd, &pt);
//    SetCursorPos(pt.x, pt.y);
//}
//
//CMainCamera* CMainCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
//{
//    CMainCamera* pInstance = new CMainCamera(pDevice, pContext, eObjectID);
//
//    if (FAILED(pInstance->Initialize_Prototype()))
//    {
//        MSG_BOX("Create Failed : MainCamera");
//        Safe_Release(pInstance);
//    }
//
//    return pInstance;
//}
//
//CGameObject* CMainCamera::Clone(void* pArg)
//{
//    CMainCamera* pInstance = new CMainCamera(*this);
//
//    if (FAILED(pInstance->Initialize(pArg)))
//    {
//        MSG_BOX("Clones Failed : MainCamera");
//        Safe_Release(pInstance);
//    }
//
//    return pInstance;
//}
//
//void CMainCamera::Free()
//{
//    __super::Free();
//
//    Safe_Release(m_pGameManager);
//    Safe_Release(m_pPlayerTransform);
//}
//
//
//void CMainCamera::Look_Target(_float fTimeDelta)
//{
//    CTransform* pTargetTransform = m_pGameManager->Calc_Target(m_pPlayerTransform->Get_State(STATE::POSITION));
//
//    if (nullptr != pTargetTransform)
//    {
//        _vector vPlayerPos = m_pPlayerTransform->Get_State(STATE::POSITION);
//        _vector vPlayerLook = m_pPlayerTransform->Get_State(STATE::LOOK);
//        _vector vCameraPos = XMLoadFloat4(m_pGameInstance->Get_CamState(STATE::POSITION));
//        _vector vCameraLook = XMVectorSetY(XMLoadFloat4(m_pGameInstance->Get_CamState(STATE::LOOK)), 0.f);
//
//        _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
//
//        _float fPlayerDot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vPlayerLook), XMVector3Normalize(XMVectorSetY(vTargetPos - vPlayerPos, 0.f))));
//        _float fCameraDot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMVectorSetY(vCameraLook, 0.f)), XMVector3Normalize(XMVectorSetY(vTargetPos - vCameraPos, 0.f))));
//
//        if ((fPlayerDot > 0.f) && (fCameraDot > 0.f))
//        {
//            _vector vLookPos = m_pPlayerTransform->Get_State(STATE::POSITION) + 2.f * XMVector3Normalize(pTargetTransform->Get_State(STATE::POSITION) - m_pPlayerTransform->Get_State(STATE::POSITION)
//                + XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::UP)) * 1.f);
//            m_pTransformCom->LookAt_Lerp(vLookPos, 0.175f);
//        }
//        else
//        {
//            m_pTransformCom->LookAt_Lerp(
//                m_pPlayerTransform->Get_State(STATE::POSITION) +
//                XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::LOOK)) * 1.f +
//                XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::UP)) * 1.f, 0.175f);
//        }
//    }
//    else
//    {
//        /* 플레이어의 look이 카메라의 look (xz 평면상)과 크게 차이날때만 lerp로 바라보게끔 처리 */
//        _vector PlayerLook = XMVectorSetY(m_pPlayerTransform->Get_State(STATE::LOOK), 0.f);
//        _vector CameraLook = XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f);
//
//
//        if (!(XMVectorGetX(XMVector3Dot(CameraLook, PlayerLook)) >= 0.1f))
//        {
//            m_pTransformCom->LookAt_Lerp(
//                m_pPlayerTransform->Get_State(STATE::POSITION) +
//                XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::LOOK)) * 1.f +
//                XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::UP)) * 1.f, 0.175f);
//        }
//    }
//}
//
//void CMainCamera::Chase_Target(_float fTimeDelta)
//{
//    _float fMouseMoveX = (_float)m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::X) / g_iWinSizeX;
//    _float fMouseMoveY = (_float)m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::Y) / g_iWinSizeY;
//
//    // 회전할 벡터와 각도
//    _vector  StartVector = XMVectorSet(0.f, 2.5f, -3.0f, 0.f);
//
//    m_fRotateX += XMConvertToRadians(fMouseMoveX * 90.f);
//    m_fRotateX = XMScalarModAngle(m_fRotateX);
//
//    m_fRotateY += XMConvertToRadians(fMouseMoveY * 45.f);
//    m_fRotateY = XMScalarModAngle(m_fRotateY);
//
//    // 라디안 제한 ( -90  ~ +90 + 여기에 캐릭터의 Look 벡터까지. )
//    _float fLimit = XMConvertToRadians(15.f);
//
//    if (m_fRotateY > fLimit)
//    m_fRotateY = fLimit;
//
//    if (m_fRotateY < -fLimit)
//        m_fRotateY = -fLimit;
//
//    _vector		vQuternion = XMQuaternionRotationRollPitchYaw(m_fRotateY, m_fRotateX, 0.f);
//
//    _matrix		RotationMatrix = XMMatrixRotationQuaternion(vQuternion);
//    _vector     vCamPos = XMVector3Rotate(StartVector, vQuternion);
//    _float      fLength = XMVectorGetX(XMVector3Length(vCamPos));
//
//    _float fDist = XMVectorGetX(XMVector3Length(m_pPlayerTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION)));
//
//    m_pTransformCom->Chase_Lerp(m_pPlayerTransform->Get_State(STATE::POSITION) + vCamPos, 0.25f * fTimeDelta, 0.f);
//}
