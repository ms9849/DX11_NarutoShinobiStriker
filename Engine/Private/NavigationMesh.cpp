#include "NavigationMesh.h"

#include "Cell.h"
#include "Shader.h"
#include "GameInstance.h"

CNavigationMesh::CNavigationMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CNavigation { pDevice, pContext }
{
}

CNavigationMesh::CNavigationMesh(const CNavigationMesh& rhs)
    : CNavigation { rhs }
{
}

void CNavigationMesh::Compute_Height(class CTransform* pTransform)
{
    _vector		vLocalPos = XMVector3TransformCoord(pTransform->Get_State(STATE::POSITION), XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));


    _float		fHeight = m_Cells[m_iCurrentCellIndex]->Compute_Height(vLocalPos);

    vLocalPos = XMVectorSetY(vLocalPos, fHeight);


    pTransform->Set_State(STATE::POSITION, XMVector3TransformCoord(vLocalPos, XMLoadFloat4x4(&m_WorldMatrix)));
}

HRESULT CNavigationMesh::Initialize_Prototype(const _char* pFilePath, _fmatrix PreTransformMatrix)
{
    _uint			iFlag = {};

    iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast | aiProcess_PreTransformVertices;

    m_pAIScene = m_Importer.ReadFile(pFilePath, iFlag);
    if (nullptr == m_pAIScene)
        return E_FAIL;

    const aiMesh* pAIMesh = m_pAIScene->mMeshes[0];

    /* 삼각형의 갯수만큼 돌면서 삼각형 전체 순회 */
    /* i번째 면의 정점 위치를 계속 가져와서 셀에 세팅한다.*/

    /* 9월 11일 기준으로 위치 보정은 되어 있지 않으므로, 이에 대한 보정이 필요하다. */
    for (_uint i = 0; i < pAIMesh->mNumFaces; ++i)
    {
        _float3		vPoints[ENUM_CLASS(NAVI_POINT::END)] = {};

        memcpy(&vPoints[0], &pAIMesh->mVertices[pAIMesh->mFaces[i].mIndices[0]], sizeof(_float3));
        XMStoreFloat3(&vPoints[0], XMVector3TransformCoord(XMLoadFloat3(&vPoints[0]), PreTransformMatrix));
        
        memcpy(&vPoints[1], &pAIMesh->mVertices[pAIMesh->mFaces[i].mIndices[1]], sizeof(_float3));
        XMStoreFloat3(&vPoints[1], XMVector3TransformCoord(XMLoadFloat3(&vPoints[1]), PreTransformMatrix));

        memcpy(&vPoints[2], &pAIMesh->mVertices[pAIMesh->mFaces[i].mIndices[2]], sizeof(_float3));
        XMStoreFloat3(&vPoints[2], XMVector3TransformCoord(XMLoadFloat3(&vPoints[2]), PreTransformMatrix));

        CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_uint)m_Cells.size());
        if (nullptr == pCell)
            return E_FAIL;

        m_Cells.push_back(pCell);
    }

#ifdef _DEBUG
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;
#endif

    SetUp_Neighbors();

    return S_OK;
}

HRESULT CNavigationMesh::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return S_OK;

    NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);

    /* 현재 타고 있는 셀의 인덱스를 구해낸다. */
    m_iCurrentCellIndex = pDesc->iCurrentCellIndex;

    return S_OK;
}

_bool CNavigationMesh::isMove(_fvector vPosition)
{
    _vector		vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

    _int		iNeighborIndex = { -1 };

    _bool		isIn = m_Cells[m_iCurrentCellIndex]->isIn(vLocalPos, &iNeighborIndex);

    if (true == isIn)
    {
        return true;
    }
    else
    {
        /* 나간 방향에 이웃이 있냐? */
        if (-1 != iNeighborIndex)
        {
            m_iCurrentCellIndex = iNeighborIndex;
            return true;
        }
        else
            return false;
    }
}

void CNavigationMesh::SetUp_Neighbors()
{
    for (auto& pSourCell : m_Cells)
    {
        for (auto& pDestCell : m_Cells)
        {
            if (pSourCell == pDestCell)
                continue;

            if (true == pDestCell->Compare(pSourCell->Get_Point(NAVI_POINT::A), pSourCell->Get_Point(NAVI_POINT::B)))
            {
                pSourCell->Set_Neighbor(NAVI_LINE::AB, pDestCell);
            }

            if (true == pDestCell->Compare(pSourCell->Get_Point(NAVI_POINT::B), pSourCell->Get_Point(NAVI_POINT::C)))
            {
                pSourCell->Set_Neighbor(NAVI_LINE::BC, pDestCell);
            }

            if (true == pDestCell->Compare(pSourCell->Get_Point(NAVI_POINT::C), pSourCell->Get_Point(NAVI_POINT::A)))
            {
                pSourCell->Set_Neighbor(NAVI_LINE::CA, pDestCell);
            }
        }
    }
}

CNavigationMesh* CNavigationMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pFilePath, _fmatrix PreTransformMatrix)
{
    CNavigationMesh* pInstance = new CNavigationMesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pFilePath, PreTransformMatrix)))
    {
        MSG_BOX("Create Failed : Navigation Mesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CNavigationMesh::Clone(void* pArg)
{
    CNavigationMesh* pInstance = new CNavigationMesh(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : Navigation Mesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNavigationMesh::Free()
{
    __super::Free();

    m_Importer.FreeScene();
}
