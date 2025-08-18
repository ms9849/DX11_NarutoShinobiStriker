#include "Picking.h"

#include "GameInstance.h"

CPicking::CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
    , m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPicking::Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    m_hWnd = hWnd;
    m_iWinSizeX = iWinSizeX;
    m_iWinSizeY = iWinSizeY;

    return S_OK;
}

void CPicking::Update()
{
    /* 월드상에서의 마우스의 정보를 구한다 .*/

    /* 뷰포트 상의 마우스 좌표를 구한다. */
    POINT       ptMouse = {};
    GetCursorPos(&ptMouse);
    ScreenToClient(m_hWnd, &ptMouse);

    _float4         vProjSpaceMousePos{};

    /* 투영스페이스 상의 마우스 좌표를 구한다. (0, 0) -> (-1, 1) (wsx,wsy) -> (1, -1) */
    vProjSpaceMousePos.x = ptMouse.x / (m_iWinSizeX * 0.5f) - 1.f;
    vProjSpaceMousePos.y = ptMouse.y / -(m_iWinSizeY * 0.5f) + 1.f;
    vProjSpaceMousePos.z = 0.f; /* near를 클릭한 것이기 때문에 .*/
    vProjSpaceMousePos.w = 1.f; /* w로 나눠서 결정된 값이기 대문에. */

    /* 뷰스페이스 상의 마우스 좌표를 구한다. */
    _vector vViewSpaceMousePos = XMVector4Transform(XMLoadFloat4(&vProjSpaceMousePos) , m_pGameInstance->Get_PipeLine_InverseMatrix(D3DTS::PROJ));

    _float3         RayPos{}, RayDir{};

    /*
    뷰 스페이스 상의 카메라는 0,0,0 에서 바라봄.

    레이의 위치는 0,0,0 에서 출발해야 한다.
    레이의 방향은 투영 평면상의 점을 뷰 스페이스로 끌어들인, 3차원 상의 점으로의 방향으로 세팅한다.
    */
    RayPos = _float3(0.f, 0.f, 0.f);
    RayDir = _float3(XMVectorGetX(vViewSpaceMousePos), XMVectorGetY(vViewSpaceMousePos), XMVectorGetZ(vViewSpaceMousePos));

    /* 월드스페이스 상의 마우스 좌표(Pos, Dir)를 구한다. */

    XMStoreFloat3(&RayPos, XMVector3TransformCoord(XMLoadFloat3(&RayPos), m_pGameInstance->Get_PipeLine_InverseMatrix(D3DTS::VIEW)));
    XMStoreFloat3(&RayDir, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&RayDir), m_pGameInstance->Get_PipeLine_InverseMatrix(D3DTS::VIEW))));

    m_vRayPos[ENUM_CLASS(RAY::WORLD)] = RayPos;
    m_vRayDir[ENUM_CLASS(RAY::WORLD)] = RayDir;
}

void CPicking::Transform_ToLocalSpace(const _float4x4* pWorldMatrixInverse)
{
}

_bool CPicking::Picking_InWorldSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut)
{
    return true;
}

_bool CPicking::Picking_InLocalSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut)
{
    return true;
}

CPicking* CPicking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWinSizeX, _uint iWinSizeY, HWND hWnd)
{
    CPicking* pInstance = new CPicking(pDevice, pContext);

    if (FAILED(pInstance->Initialize(hWnd, iWinSizeX, iWinSizeY)))
    {
        MSG_BOX("Create Failed : Picking");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPicking::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
