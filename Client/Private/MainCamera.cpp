#include "MainCamera.h"

#include "GameManager.h"
#include "GameInstance.h"

CMainCamera::CMainCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CCamera { pDevice, pContext, ENUM_CLASS(eObjectID) }
    , m_pGameManager { CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

CMainCamera::CMainCamera(const CMainCamera& rhs)
    : CCamera{ rhs }
    , m_pGameManager { CGameManager::GetInstance() }
{
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
    _float fMouseMoveX = (_float)m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::X) / g_iWinSizeX;
    _float fMouseMoveY = (_float)m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::Y) / g_iWinSizeY;

    // 회전할 벡터와 각도
    _vector  StartVector = XMVectorSet(1.f, 2.5f, -3.f, 0.f);

    /* 플레이어 기준으로 누적된 위치에 세팅*/
    //    _vector  StartVector;
    //StartVector = XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::LOOK)) * -4.f;
    //StartVector += XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::UP)) * 3.f;
    //StartVector += XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::RIGHT)) * 1.f;

    /* 스타트 벡터에 따라 다르게 제한이 들어가야 하는데.. */
    m_fRotateX += XMConvertToRadians(fMouseMoveX * 180.f);
    m_fRotateX = fmod(m_fRotateX, XM_2PI);

    m_fRotateY += XMConvertToRadians(fMouseMoveY * 180.f);
    m_fRotateY = fmod(m_fRotateY, XM_2PI);

    // 라디안 제한 ( -90  ~ +90 + 여기에 캐릭터의 Look 벡터까지. )
    _float fLimit = XMConvertToRadians(89.f);

    /* Y 라디안 제한 */
    if (m_fRotateY > fLimit)
        m_fRotateY = fLimit;

    if (m_fRotateY < -fLimit)
        m_fRotateY = -fLimit;

    ///* X 라디안 제한 */
    //if (m_fRotateX > fLimit) 
    //    m_fRotateX = fLimit;

    //if (m_fRotateX < -fLimit) 
    //    m_fRotateX = -fLimit;

    _vector		vQuternion = XMQuaternionRotationRollPitchYaw(/*m_fRotateY*/ 0.f, m_fRotateX, 0.f);

    _matrix		RotationMatrix = XMMatrixRotationQuaternion(vQuternion);
    _vector     vCamPos = XMVector3TransformNormal(StartVector, RotationMatrix);

    _float      fLength = XMVectorGetX(XMVector3Length(vCamPos));

    m_pTransformCom->Chase_Lerp(m_pPlayerTransform->Get_State(STATE::POSITION) + vCamPos, fTimeDelta * 2.0f, 0.f);
    m_pTransformCom->LookAt_Lerp(m_pPlayerTransform->Get_State(STATE::POSITION) + XMVector3Normalize(m_pPlayerTransform->Get_State(STATE::LOOK)) * 1.f);

    // 클라이언트 영역 크기 얻기
    RECT rcClient;
    GetClientRect(g_hWnd, &rcClient);

    _int clientCenterX = (rcClient.right - rcClient.left) / 2;
    _int clientCenterY = (rcClient.bottom - rcClient.top) / 2;

    // 클라이언트 좌표 → 스크린 좌표로 변환
    POINT pt = { clientCenterX, clientCenterY };
    ClientToScreen(g_hWnd, &pt);
    SetCursorPos(pt.x, pt.y);

    __super::Bind_Matrices();
}

void CMainCamera::Update(_float fTimeDelta)
{
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
