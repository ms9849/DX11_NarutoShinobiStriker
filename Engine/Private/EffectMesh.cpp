#include "EffectMesh.h"

#include "GameInstance.h"

CEffectMesh::CEffectMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer { pDevice, pContext }
{
}

CEffectMesh::CEffectMesh(const CEffectMesh& rhs)
    : CVIBuffer{ rhs }
{
}

_wstring CEffectMesh::Get_Name() const
{
    return m_pGameInstance->ToWstring(string(m_szName));
}

/* Assimp를 이용한 로딩 */
HRESULT CEffectMesh::Initialize_Prototype(MODEL eType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
    strcpy_s(m_szName, pAIMesh->mName.data);

    m_iNumVertexBuffers = 1;
    m_iNumVertices = pAIMesh->mNumVertices;

    m_iNumIndices = pAIMesh->mNumFaces * 3;
    m_iIndexStride = 4;

    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX

    HRESULT hr = Ready_VertexBuffer_For_NonAnim_Assimp(pAIMesh, PreTransformMatrix);
    if (FAILED(hr))
        return E_FAIL;

#pragma endregion


#pragma region INDEX
    D3D11_BUFFER_DESC		IBDesc{};
    IBDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
    IBDesc.Usage = D3D11_USAGE_DEFAULT;
    IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IBDesc.StructureByteStride = m_iIndexStride;
    IBDesc.CPUAccessFlags = 0;
    IBDesc.MiscFlags = 0;


    _uint* pIndices = new _uint[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

    _uint	iNumIndices = {};

    for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
    {

        pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
        pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
        pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
    }


    D3D11_SUBRESOURCE_DATA	InitialIBData{};
    InitialIBData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitialIBData, &m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);
#pragma endregion

    return S_OK;
}

/* Binary를 이용한 로딩 */
HRESULT CEffectMesh::Initialize_Prototype(MODEL eType, HANDLE hHandle, DWORD* dwByte, _fmatrix PreTransformMatrix)
{
    m_iNumVertexBuffers = 1;
    m_iIndexStride = 4;

    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    if (FAILED(Load_Mesh_FromBinary(hHandle, dwByte, eType)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffectMesh::Initialize(void* pArg)
{
    return S_OK;
}

/* Binary화 */
HRESULT CEffectMesh::Save_Mesh_ToBinary(HANDLE hHandle, DWORD* dwByte, const aiMesh* pAIMesh) const
{
    /*
      * pBoneMatrices는 할당만 해주면 됨. (m_iNumBones에 맞춰서
      * 버텍스 포지션도 마찬가지로 m_pVB에서 받아오면 됨
      */

    if (false == WriteFile(hHandle, &m_szName, MESH_MAX, dwByte, nullptr))
        return E_FAIL;

    if (false == WriteFile(hHandle, &m_iVertexStride, sizeof(_uint), dwByte, nullptr))
        return E_FAIL;

    if (false == WriteFile(hHandle, &m_iNumVertices, sizeof(_uint), dwByte, nullptr))
        return E_FAIL;

    if (false == WriteFile(hHandle, &m_iNumIndices, sizeof(_uint), dwByte, nullptr))
        return E_FAIL;

    if (false == WriteFile(hHandle, &m_iIndexStride, sizeof(_uint), dwByte, nullptr))
        return E_FAIL;

    if (false == WriteFile(hHandle, &m_eIndexFormat, sizeof(DXGI_FORMAT), dwByte, nullptr))
        return E_FAIL;

    /* 정점 정보 저장 */
    D3D11_BUFFER_DESC VBDesc{};
    m_pVB->GetDesc(&VBDesc);

    D3D11_BUFFER_DESC StagingDesc = VBDesc;
    StagingDesc.Usage = D3D11_USAGE_STAGING;
    StagingDesc.BindFlags = 0;
    StagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    StagingDesc.MiscFlags = 0;

    ID3D11Buffer* pStagingVB = {};
    if (FAILED(m_pDevice->CreateBuffer(&StagingDesc, nullptr, &pStagingVB)))
        return E_FAIL;

    m_pContext->CopyResource(pStagingVB, m_pVB);

    D3D11_MAPPED_SUBRESOURCE StagingData{};
    if (FAILED(m_pContext->Map(pStagingVB, 0, D3D11_MAP_READ, 0, &StagingData)))
        return E_FAIL;

    WriteFile(hHandle, StagingData.pData, VBDesc.ByteWidth, dwByte, nullptr);

    m_pContext->Unmap(pStagingVB, 0);

    Safe_Release(pStagingVB);

    /* 인덱스 정보 저장 */
    _uint Indices[3];
    for (_uint j = 0; j < m_iNumIndices / 3; ++j)
    {
        Indices[0] = pAIMesh->mFaces[j].mIndices[0];
        Indices[1] = pAIMesh->mFaces[j].mIndices[1];
        Indices[2] = pAIMesh->mFaces[j].mIndices[2];

        WriteFile(hHandle, &Indices, sizeof(_uint) * 3, dwByte, nullptr);
    }

    return S_OK;
}

/* Binary 로딩 */
HRESULT CEffectMesh::Load_Mesh_FromBinary(HANDLE hHandle, DWORD* dwByte, MODEL eType)
{
    /*
      * pBoneMatrices는 할당만 해주면 됨. (m_iNumBones에 맞춰서
      * 버텍스 포지션도 마찬가지로 정점 정보에서 받아오면 됨
      */

    if (false == ReadFile(hHandle, &m_szName, MESH_MAX, dwByte, nullptr))
        return E_FAIL;

    if (false == ReadFile(hHandle, &m_iVertexStride, sizeof(_uint), dwByte, nullptr))
        return E_FAIL;

    if (false == ReadFile(hHandle, &m_iNumVertices, sizeof(_uint), dwByte, nullptr))
        return E_FAIL;

    if (false == ReadFile(hHandle, &m_iNumIndices, sizeof(_uint), dwByte, nullptr))
        return E_FAIL;

    if (false == ReadFile(hHandle, &m_iIndexStride, sizeof(_uint), dwByte, nullptr))
        return E_FAIL;

    if (false == ReadFile(hHandle, &m_eIndexFormat, sizeof(DXGI_FORMAT), dwByte, nullptr))
        return E_FAIL;

#pragma region VERTEX
    /* 정점 정보 로딩 */
    HRESULT hr = Ready_VertexBuffer_For_NonAnim_Binary(hHandle, dwByte);

    if (FAILED(hr))
        return E_FAIL;
#pragma endregion

#pragma region INDEX
    D3D11_BUFFER_DESC IBDesc;
    IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
    IBDesc.Usage = D3D11_USAGE_DEFAULT;
    IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IBDesc.StructureByteStride = m_iIndexStride;
    IBDesc.CPUAccessFlags = 0;
    IBDesc.MiscFlags = 0;

    _uint* pIndices = new _uint[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

    _uint iNumIndices = {};
    _uint Indices[3];

    /* 인덱스 정보 로딩 */
    for (_uint j = 0; j < m_iNumIndices / 3; ++j)
    {
        if (FAILED(ReadFile(hHandle, &Indices, sizeof(_uint) * 3, dwByte, nullptr)))
            return E_FAIL;

        pIndices[iNumIndices++] = Indices[0];
        pIndices[iNumIndices++] = Indices[1];
        pIndices[iNumIndices++] = Indices[2];
    }

    D3D11_SUBRESOURCE_DATA InitialIBData{};
    InitialIBData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitialIBData, &m_pIB)))
        return E_FAIL;
#pragma endregion

    Safe_Delete_Array(pIndices);

    return S_OK;
}

/* Assimp Vertex 로딩 */
HRESULT CEffectMesh::Ready_VertexBuffer_For_NonAnim_Assimp(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
    m_iVertexStride = sizeof(VTXMESH);
    D3D11_BUFFER_DESC	VBDesc;
    VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    VBDesc.Usage = D3D11_USAGE_DEFAULT;
    VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VBDesc.StructureByteStride = m_iVertexStride;
    VBDesc.CPUAccessFlags = 0;
    VBDesc.MiscFlags = 0;

    VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

    m_pVertexPositions = new _float3[m_iNumVertices];
    ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

    for (_uint i = 0; i < m_iNumVertices; ++i)
    {
        memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));
        memcpy(&m_pVertexPositions[i], &pVertices[i], sizeof(_float3));

        memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix)));

        memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vTangent, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix)));

        memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
    }

    D3D11_SUBRESOURCE_DATA	InitialVBData{};
    InitialVBData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    return S_OK;
}

HRESULT CEffectMesh::Ready_VertexBuffer_For_NonAnim_Binary(HANDLE hHandle, DWORD* dwByte)
{
    m_iVertexStride = sizeof(VTXMESH);
    D3D11_BUFFER_DESC		VBDesc{};
    VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    VBDesc.Usage = D3D11_USAGE_DEFAULT;
    VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VBDesc.StructureByteStride = m_iVertexStride;
    VBDesc.CPUAccessFlags = 0;
    VBDesc.MiscFlags = 0;

    VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

    m_pVertexPositions = new _float3[m_iNumVertices];
    ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

    for (_uint i = 0; i < m_iNumVertices; ++i)
    {
        if (false == (ReadFile(hHandle, &pVertices[i], sizeof(VTXMESH), dwByte, nullptr)))
            return E_FAIL;

        memcpy(&m_pVertexPositions[i], &pVertices[i], sizeof(_float3));
    }

    D3D11_SUBRESOURCE_DATA	InitialVBData{};
    InitialVBData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    return S_OK;
}

CEffectMesh* CEffectMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
    CEffectMesh* pInstance = new CEffectMesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eType, pAIMesh, PreTransformMatrix)))
    {
        MSG_BOX("Create Failed : CEffectMesh By Assimp");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CEffectMesh* CEffectMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, HANDLE hHandle, DWORD* dwByte, _fmatrix PreTransformMatrix)
{
    CEffectMesh* pInstance = new CEffectMesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eType, hHandle, dwByte, PreTransformMatrix)))
    {
        MSG_BOX("Create Failed : CEffectMesh By Binary");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CEffectMesh::Clone(void* pArg)
{
    CEffectMesh* pInstance = new CEffectMesh(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CEffectMesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffectMesh::Free()
{
    __super::Free();
}
