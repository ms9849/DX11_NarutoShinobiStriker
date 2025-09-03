#include "Mesh.h"

#include "GameInstance.h"
#include "Model.h"
#include "Bone.h"
#include "Shader.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer { pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& rhs)
    : CVIBuffer { rhs }
{
}

_wstring CMesh::Get_Name() const
{
    return m_pGameInstance->ToWstring(string(m_szName));
}

_bool CMesh::Picking(_fmatrix WolrdMatrixInverse, _float3* pOut)
{
    m_pGameInstance->Transform_Picking_ToLocalSpace(WolrdMatrixInverse);

    _uint   iNumIndices = {0};
    _uint   iIndex[3];
    _float  fMinDist = FLT_MAX;
    _float3 vMinOut;
    _float4 vCamPos = *m_pGameInstance->Get_CamState(STATE::POSITION);

    for (_uint i = 0; i < m_iNumIndices / 3; ++i)
    {
        iIndex[0] = *((_uint*)(m_StagingData.pData) + iNumIndices++);
        iIndex[1] = *((_uint*)(m_StagingData.pData) + iNumIndices++);
        iIndex[2] = *((_uint*)(m_StagingData.pData) + iNumIndices++);

        if (true == m_pGameInstance->Picking_InLocalSpace(XMLoadFloat3(&m_pVertexPositions[iIndex[2]]), XMLoadFloat3(&m_pVertexPositions[iIndex[1]]), XMLoadFloat3(&m_pVertexPositions[iIndex[0]]), pOut))
        {
            _float fDist = XMVectorGetX(XMVector4Length(XMVectorSet(pOut->x, pOut->y, pOut->z, 1.f) - XMLoadFloat4(&vCamPos)));
            
            if (fDist < fMinDist)
            {
                fMinDist = fDist;
                vMinOut = *pOut;
            }
        }
    }

    if (fMinDist != FLT_MAX)
    {
        *pOut = vMinOut;
        return true;
    }

    return false;
}

HRESULT CMesh::Initialize_Prototype(MODEL eType, const class CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
    strcpy_s(m_szName, pAIMesh->mName.data);
 
    /* 몇 번째 머테리얼을 가져다 쓰는지 저장하기 위함. */
    m_iMaterialIndex = pAIMesh->mMaterialIndex;
    m_iNumVertexBuffers = 1;
    m_iNumVertices = pAIMesh->mNumVertices;

    m_iNumIndices = pAIMesh->mNumFaces * 3;
    m_iIndexStride = 4;

    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX

    HRESULT hr = MODEL::NONANIM == eType ?
        Ready_VertexBuffer_For_NonAnim_Assimp(pAIMesh, PreTransformMatrix) :
        Ready_VertexBuffer_For_Anim_Assimp(pModel, pAIMesh);

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

    for (_uint i = 0; i < pAIMesh->mNumFaces; ++i)
    {
        pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
        pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
        pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
    }

    D3D11_SUBRESOURCE_DATA InitialIBData{};
    InitialIBData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitialIBData, &m_pIB)))
        return E_FAIL;
    
    Safe_Delete_Array(pIndices);
#pragma endregion

    D3D11_BUFFER_DESC StagingDesc{};
    m_pIB->GetDesc(&StagingDesc);
    StagingDesc.Usage = D3D11_USAGE_STAGING;
    StagingDesc.BindFlags = 0;
    StagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    StagingDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateBuffer(&StagingDesc, nullptr, &m_pStagingIB)))
        return E_FAIL;

    //이거 뺴야된다 무조건.
    m_pContext->CopyResource(m_pStagingIB, m_pIB);

    if (FAILED(m_pContext->Map(m_pStagingIB, 0, D3D11_MAP_READ, 0, &m_StagingData)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMesh::Initialize_Prototype(MODEL eType, const CModel* pModel, HANDLE hHandle, DWORD* dwByte, _fmatrix PreTransformMatrix)
{
    m_iNumVertexBuffers = 1;
    m_iIndexStride = 4;

    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    if (FAILED(Load_Mesh_FromBinary(hHandle, dwByte, eType)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
    D3D11_BUFFER_DESC StagingDesc{};
    m_pIB->GetDesc(&StagingDesc);

    StagingDesc.Usage = D3D11_USAGE_STAGING;
    StagingDesc.BindFlags = 0;
    StagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    StagingDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateBuffer(&StagingDesc, nullptr, &m_pStagingIB)))
        return E_FAIL;

    m_pContext->CopyResource(m_pStagingIB, m_pIB);
    D3D11_MAPPED_SUBRESOURCE StagingData{};

    if (FAILED(m_pContext->Map(m_pStagingIB, 0, D3D11_MAP_READ, 0, &StagingData)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(const vector<class CBone*>& Bones, CShader* pShader, const _char* pConstantName)
{
    if (m_iNumBones >= 512)
        return E_FAIL;


    for (size_t i = 0; i < m_iNumBones; i++)
    {
        /* 최종적으로 렌더링하기위한 뼈의 행렬(CombinedTransformationMatrix). */
        XMStoreFloat4x4(&m_pBoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
    }

    if (0 == m_iNumBones)
        return S_OK;

    return pShader->Bind_Matrices(pConstantName, m_pBoneMatrices, m_iNumBones);
}

HRESULT CMesh::Save_Mesh_ToBinary(HANDLE hHandle, DWORD* dwByte, const aiMesh* pAIMesh) const
{
    /*
    * pBoneMatrices는 할당만 해주면 됨. (m_iNumBones에 맞춰서
    * 버텍스 포지션도 마찬가지로 m_pVB에서 받아오면 됨
    */

    if (false == WriteFile(hHandle, &m_szName, MESH_MAX, dwByte, nullptr))
        return E_FAIL;

    if (false == WriteFile(hHandle, &m_iMaterialIndex, sizeof(_uint), dwByte, nullptr))
        return E_FAIL;

    if (false == WriteFile(hHandle, &m_iNumBones, sizeof(_uint), dwByte, nullptr))
        return E_FAIL;

    for(_uint i = 0; i < m_iNumBones; ++i)
        if (false == WriteFile(hHandle, &m_BoneIndices[i], sizeof(_uint), dwByte, nullptr))
            return E_FAIL;

    for(_uint i = 0; i < m_iNumBones; ++i)
        if (false == WriteFile(hHandle, &m_OffsetMatrices[i], sizeof(_float4x4), dwByte, nullptr))
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
    if(FAILED(m_pContext->Map(pStagingVB, 0, D3D11_MAP_READ, 0, &StagingData)))
        return E_FAIL;

    WriteFile(hHandle, StagingData.pData, VBDesc.ByteWidth, dwByte, nullptr);
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

HRESULT CMesh::Load_Mesh_FromBinary(HANDLE hHandle, DWORD* dwByte, MODEL eType)
{
    /*
    * pBoneMatrices는 할당만 해주면 됨. (m_iNumBones에 맞춰서
    * 버텍스 포지션도 마찬가지로 정점 정보에서 받아오면 됨
    */

    if (false == ReadFile(hHandle, &m_szName, MESH_MAX, dwByte, nullptr))
        return E_FAIL;

    if (false == ReadFile(hHandle, &m_iMaterialIndex, sizeof(_uint), dwByte, nullptr))
        return E_FAIL;

    if (false == ReadFile(hHandle, &m_iNumBones, sizeof(_uint), dwByte, nullptr))
        return E_FAIL;

    m_pBoneMatrices = new _float4x4[m_iNumBones];

    m_BoneIndices.reserve(m_iNumBones);
    for (_uint i = 0; i < m_iNumBones; ++i)
    {
        _uint iData;

        if (false == ReadFile(hHandle, &iData, sizeof(_uint), dwByte, nullptr))
            return E_FAIL;

        m_BoneIndices.push_back(iData);
    }

    m_OffsetMatrices.reserve(m_iNumBones);
    for (_uint i = 0; i < m_iNumBones; ++i)
    {
        _float4x4 Matrix;

        if (false == ReadFile(hHandle, &Matrix, sizeof(_float4x4), dwByte, nullptr))
            return E_FAIL;

        m_OffsetMatrices.push_back(Matrix);
    }

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
    HRESULT hr = m_iVertexStride == sizeof(VTXANIMMESH) ? 
        Ready_VertexBuffer_For_Anim_Binary(hHandle, dwByte) : Ready_VertexBuffer_For_NonAnim_Binary(hHandle, dwByte);

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
        if(FAILED(ReadFile(hHandle, &Indices, sizeof(_uint) * 3, dwByte, nullptr)))
            return E_FAIL;

        pIndices[iNumIndices++] = Indices[0];
        pIndices[iNumIndices++] = Indices[1];
        pIndices[iNumIndices++] = Indices[2];
    }

    D3D11_SUBRESOURCE_DATA InitialIBData{};
    InitialIBData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitialIBData, &m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);
#pragma endregion

    D3D11_BUFFER_DESC StagingDesc{};
    m_pIB->GetDesc(&StagingDesc);
    StagingDesc.Usage = D3D11_USAGE_STAGING;
    StagingDesc.BindFlags = 0;
    StagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    StagingDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateBuffer(&StagingDesc, nullptr, &m_pStagingIB)))
        return E_FAIL;

    m_pContext->CopyResource(m_pStagingIB, m_pIB);

    if (FAILED(m_pContext->Map(m_pStagingIB, 0, D3D11_MAP_READ, 0, &m_StagingData)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_NonAnim_Assimp(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
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

HRESULT CMesh::Ready_VertexBuffer_For_Anim_Assimp(const CModel* pModel, const aiMesh* pAIMesh)
{
    m_iVertexStride = sizeof(VTXANIMMESH);
    D3D11_BUFFER_DESC		VBDesc{};
    VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    VBDesc.Usage = D3D11_USAGE_DEFAULT;
    VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VBDesc.StructureByteStride = m_iVertexStride;
    VBDesc.CPUAccessFlags = 0;
    VBDesc.MiscFlags = 0;

    VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

    m_pVertexPositions = new _float3[m_iNumVertices];
    ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

    for (_uint i = 0; i < m_iNumVertices; ++i)
    {
        memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
        memcpy(&m_pVertexPositions[i], &pVertices[i], sizeof(_float3));

        memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
        memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
        memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
    }

    m_iNumBones = pAIMesh->mNumBones;

    m_OffsetMatrices.reserve(m_iNumBones);

    m_pBoneMatrices = new _float4x4[0 == m_iNumBones ? 1 : m_iNumBones];
    ZeroMemory(m_pBoneMatrices, sizeof(_float4x4) * m_iNumBones);

    _float4x4		OffsetMatrix;
    XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

    for (size_t i = 0; i < m_iNumBones; i++)
    {
        aiBone* pAIBone = pAIMesh->mBones[i];

        memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
        XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

        _int		iBoneIndex = pModel->Get_BoneIndex(pAIBone->mName.data);

        if (-1 == iBoneIndex)
            return E_FAIL;

        m_OffsetMatrices.push_back(OffsetMatrix);
        m_BoneIndices.push_back(iBoneIndex);

        /* 이 뼈는 몇개의 정점에게 영향을 주는가? */
        for (size_t j = 0; j < pAIBone->mNumWeights; j++)
        {
            aiVertexWeight	AIWeight = pAIBone->mWeights[j];

            if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.x)
            {
                pVertices[AIWeight.mVertexId].vBlendIndex.x = i;
                pVertices[AIWeight.mVertexId].vBlendWeight.x = AIWeight.mWeight;
            }

            else if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.y)
            {
                pVertices[AIWeight.mVertexId].vBlendIndex.y = i;
                pVertices[AIWeight.mVertexId].vBlendWeight.y = AIWeight.mWeight;
            }

            else if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.z)
            {
                pVertices[AIWeight.mVertexId].vBlendIndex.z = i;
                pVertices[AIWeight.mVertexId].vBlendWeight.z = AIWeight.mWeight;
            }

            else
            {
                pVertices[AIWeight.mVertexId].vBlendIndex.w = i;
                pVertices[AIWeight.mVertexId].vBlendWeight.w = AIWeight.mWeight;
            }
            /*test */
            int a = 10;
        }
    }

    if (0 == m_iNumBones)
    {
        m_iNumBones = 1;

        m_BoneIndices.push_back(pModel->Get_BoneIndex(m_szName));

        m_OffsetMatrices.push_back(OffsetMatrix);
    }

    D3D11_SUBRESOURCE_DATA	InitialVBData{};
    InitialVBData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_NonAnim_Binary(HANDLE hHandle, DWORD* dwByte)
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
        if(false == (ReadFile(hHandle, &pVertices[i], sizeof(VTXMESH), dwByte, nullptr)))
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

HRESULT CMesh::Ready_VertexBuffer_For_Anim_Binary(HANDLE hHandle, DWORD* dwByte)
{
    m_iVertexStride = sizeof(VTXANIMMESH);
    D3D11_BUFFER_DESC		VBDesc{};
    VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    VBDesc.Usage = D3D11_USAGE_DEFAULT;
    VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VBDesc.StructureByteStride = m_iVertexStride;
    VBDesc.CPUAccessFlags = 0;
    VBDesc.MiscFlags = 0;

    VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

    m_pVertexPositions = new _float3[m_iNumVertices];
    ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

    for (_uint i = 0; i < m_iNumVertices; ++i)
    {
        if (false == (ReadFile(hHandle, &pVertices[i], sizeof(VTXANIMMESH), dwByte, nullptr)))
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


CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const class CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
    CMesh* pInstance = new CMesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eType, pModel, pAIMesh, PreTransformMatrix)))
    {
        MSG_BOX("Create Failed : Mesh By Assimp");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const CModel* pModel, HANDLE hHandle, DWORD* dwByte, _fmatrix PreTransformMatrix)
{
    CMesh* pInstance = new CMesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eType, pModel, hHandle, dwByte, PreTransformMatrix)))
    {
        MSG_BOX("Create Failed : Mesh By Binary");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
    CMesh* pInstance = new CMesh(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : Mesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CMesh::Free()
{
    __super::Free();

    Safe_Delete_Array(m_pBoneMatrices);
    Safe_Release(m_pStagingIB);
}
