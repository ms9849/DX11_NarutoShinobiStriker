#include "Picking_Manager.h"

#include "GameInstance.h"

CPicking_Manager::CPicking_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
    , m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPicking_Manager::Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY, _uint iNumLevels)
{
    m_hWnd = hWnd;
    m_iWinSizeX = iWinSizeX;
    m_iWinSizeY = iWinSizeY;

    m_iNumLevels = iNumLevels;
    m_pPickingTargets = new list<pair<class CGameObject*, class CVIBuffer*>>[m_iNumLevels];
    m_pPickingModels = new list<pair<class CGameObject*, class CModel*>>[m_iNumLevels];

    return S_OK;
}

void CPicking_Manager::Update()
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
    레이의 방향은 투영 평면(2차원)상의 점을 뷰 스페이스로 끌어들인, 3차원 상의 점으로의 방향으로 세팅한다.
    */
    RayPos = _float3(0.f, 0.f, 0.f);
    RayDir = _float3(XMVectorGetX(vViewSpaceMousePos), XMVectorGetY(vViewSpaceMousePos), XMVectorGetZ(vViewSpaceMousePos));

    /* 월드스페이스 상의 마우스 좌표(Pos, Dir)를 구한다. */

    XMStoreFloat3(&RayPos, XMVector3TransformCoord(XMLoadFloat3(&RayPos), m_pGameInstance->Get_PipeLine_InverseMatrix(D3DTS::VIEW)));
    XMStoreFloat3(&RayDir, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&RayDir), m_pGameInstance->Get_PipeLine_InverseMatrix(D3DTS::VIEW))));

    m_vRayPos[ENUM_CLASS(RAY::WORLD)] = RayPos;
    m_vRayDir[ENUM_CLASS(RAY::WORLD)] = RayDir;
}

void CPicking_Manager::Transform_ToLocalSpace(_fmatrix WorldMatrixInverse)
{
    XMStoreFloat3(&m_vRayPos[ENUM_CLASS(RAY::LOCAL)], XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos[ENUM_CLASS(RAY::WORLD)]), WorldMatrixInverse)); 
    XMStoreFloat3(&m_vRayDir[ENUM_CLASS(RAY::LOCAL)], XMVector3Normalize(XMVector3TransformCoord(XMLoadFloat3(&m_vRayDir[ENUM_CLASS(RAY::WORLD)]), WorldMatrixInverse)));
}

_bool CPicking_Manager::Picking_InWorldSpace(_fvector vPointA, _fvector vPointB, _fvector vPointC, _float3* pOut)
{
    /*
    inline bool XM_CALLCONV Intersects(
    FXMVECTOR Origin, FXMVECTOR Direction, FXMVECTOR V0,
    GXMVECTOR V1,
    HXMVECTOR V2, float& Dist)
    */

    _float      fDist{};
    _bool       isColl{};

    isColl = TriangleTests::Intersects(XMLoadFloat3(&m_vRayPos[ENUM_CLASS(RAY::WORLD)]), XMLoadFloat3(&m_vRayDir[ENUM_CLASS(RAY::WORLD)]), vPointA, vPointB, vPointC, fDist);

    if (true == isColl)
    {
        XMStoreFloat3(pOut, XMLoadFloat3(&m_vRayPos[ENUM_CLASS(RAY::WORLD)]) + XMLoadFloat3(&m_vRayDir[ENUM_CLASS(RAY::WORLD)]) * fDist);
    }

    return isColl;
}

_bool CPicking_Manager::Picking_InLocalSpace(_fvector vPointA, _fvector vPointB, _fvector vPointC, _float3* pOut)
{
    _float      fDist{};
    _bool       isColl{};

    isColl = TriangleTests::Intersects(XMLoadFloat3(&m_vRayPos[ENUM_CLASS(RAY::LOCAL)]), XMLoadFloat3(&m_vRayDir[ENUM_CLASS(RAY::LOCAL)]), vPointA, vPointB, vPointC, fDist);

    if (true == isColl)
    {
        XMStoreFloat3(pOut, XMLoadFloat3(&m_vRayPos[ENUM_CLASS(RAY::LOCAL)]) + XMLoadFloat3(&m_vRayDir[ENUM_CLASS(RAY::LOCAL)]) * fDist);
    }

    return isColl;
}

_bool CPicking_Manager::Picking(_uint iLevelIdx, _float3* pOut)
{
    m_pPickingTargets[iLevelIdx].sort([](pair<CGameObject*, CVIBuffer*>(pSour), pair<CGameObject*, CVIBuffer*>(pDest))->_bool
        {
            return static_cast<CGameObject*>(pSour.first)->Get_CamDistance() > static_cast<CGameObject*>(pDest.first)->Get_CamDistance();
        });

    m_pPickingModels[iLevelIdx].sort([](pair<CGameObject*, CModel*>(pSour), pair<CGameObject*, CModel*>(pDest))->_bool
        {
            return static_cast<CGameObject*>(pSour.first)->Get_CamDistance() > static_cast<CGameObject*>(pDest.first)->Get_CamDistance();
        });


    for (auto& Pair : m_pPickingTargets[iLevelIdx])
    {
        _matrix WorldMatrixInverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(static_cast<CTransform*>(Pair.first->Find_Component(g_strTransformTag))->Get_WorldMatrixPtr()));
        
        if (Pair.second->Picking(WorldMatrixInverse, pOut))
        {
            m_pPickedObject = Pair.first;
            return true;
        }
    }

    for (auto& Pair : m_pPickingModels[iLevelIdx])
    {
        _matrix WorldMatrixInverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(static_cast<CTransform*>(Pair.first->Find_Component(g_strTransformTag))->Get_WorldMatrixPtr()));

        if (Pair.second->Picking_Meshes(WorldMatrixInverse, pOut))
        { 
            m_pPickedObject = Pair.first;
            return true;
        }
    }

    return false;
}

HRESULT CPicking_Manager::Add_GameObject_ToPicking(_uint iLevelIdx, CGameObject* pGameObject, CVIBuffer* pVIBuffer)
{
    if (iLevelIdx >= m_iNumLevels)
        return E_FAIL;

    m_pPickingTargets[iLevelIdx].push_back(make_pair(pGameObject, pVIBuffer));

    Safe_AddRef(pGameObject);
    Safe_AddRef(pVIBuffer);

    return S_OK;
}

HRESULT CPicking_Manager::Add_GameObject_ToPicking(_uint iLevelIdx, CGameObject* pGameObject, CModel* pModel)
{
    if (iLevelIdx >= m_iNumLevels)
        return E_FAIL;

    m_pPickingModels[iLevelIdx].push_back(make_pair(pGameObject, pModel));

    Safe_AddRef(pGameObject);
    Safe_AddRef(pModel);

    return S_OK;
}

void CPicking_Manager::Clear(_uint iLevelIndex)
{
    for (auto& pPair : m_pPickingTargets[iLevelIndex])
    {
        Safe_Release(pPair.first);
        Safe_Release(pPair.second);
    }

    m_pPickingTargets[iLevelIndex].clear();
}

CPicking_Manager* CPicking_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWinSizeX, _uint iWinSizeY, HWND hWnd, _uint iNumLevels)
{
    CPicking_Manager* pInstance = new CPicking_Manager(pDevice, pContext);

    if (FAILED(pInstance->Initialize(hWnd, iWinSizeX, iWinSizeY, iNumLevels)))
    {
        MSG_BOX("Create Failed : Picking");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPicking_Manager::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);

    for (_uint i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& pPair : m_pPickingTargets[i])
        {
            Safe_Release(pPair.first);
            Safe_Release(pPair.second);
        }

        m_pPickingTargets[i].clear();
    }

    for (_uint i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& pPair : m_pPickingModels[i])
        {
            Safe_Release(pPair.first);
            Safe_Release(pPair.second);
        }

        m_pPickingModels[i].clear();
    }

    Safe_Delete_Array(m_pPickingTargets);
    Safe_Delete_Array(m_pPickingModels);
}
