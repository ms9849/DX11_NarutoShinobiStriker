#include "Mesh.h"

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

HRESULT CMesh::Initialize_Prototype(MODEL eType, const class CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
    strcpy_s(m_szName, pAIMesh->mName.data);
 
    /* 몇 번째 머테리얼을 가져다 쓰는지 저장하기 위함. */
    m_iMaterialIndex = pAIMesh->mMaterialIndex;
    m_iNumVertexBuffers = 1;
    m_iNumVertices = pAIMesh->mNumVertices;
    m_iVertexStride = sizeof(VTXMESH);

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

    return S_OK;
}

/* 고칠게 많다. 본 정보도 넣어줘야 하고.. */
HRESULT CMesh::Initialize_Prototype(MODEL eType, const class CModel* pModel, const IMPORT_MESH_DESC& Desc, _fmatrix PreTransformMatrix)
{
    /* 몇 번째 머테리얼을 가져다 쓰는지 저장하기 위함. */
    m_iMaterialIndex = Desc.iMaterialIndex;
    m_iNumVertexBuffers = 1;
    m_iNumVertices = Desc.iNumVertices;
    m_iVertexStride = sizeof(VTXMESH);

    m_iNumIndices = Desc.iNumFaces * 3;
    m_iIndexStride = 4;

    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX

    HRESULT hr = MODEL::ANIM == eType ?
        Ready_VertexBuffer_For_Anim_Binary(pModel, Desc) :
        Ready_VertexBuffer_For_NonAnim_Binary(Desc, PreTransformMatrix);

    if (FAILED(hr))
        return E_FAIL;

    D3D11_BUFFER_DESC VBDesc;
    VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
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
        memcpy(&pVertices[i].vPosition, &Desc.pVertices[i].vPosition, sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

        memcpy(&pVertices[i].vNormal, &Desc.pVertices[i].vNormal, sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix)));

        memcpy(&pVertices[i].vTangent, &Desc.pVertices[i].vTangent, sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vTangent, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix)));

        memcpy(&pVertices[i].vTexcoord, &Desc.pVertices[i].vTexcoord, sizeof(_float2));

    }

    D3D11_SUBRESOURCE_DATA	InitialVBData{};
    InitialVBData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);
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

    _uint iNumIndices = { 0 };

    for (_uint i = 0; i < Desc.iNumFaces; ++i)
    {
        memcpy(&pIndices[iNumIndices], &Desc.pIndices[iNumIndices], sizeof(_uint) * 3);

        iNumIndices += 3;
    }

    D3D11_SUBRESOURCE_DATA InitialIBData{};
    InitialIBData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitialIBData, &m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);
#pragma endregion


    return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
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

HRESULT CMesh::Ready_VertexBuffer_For_NonAnim_Assimp(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
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
        memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

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
                pVertices[AIWeight.mVertexId].vBlendIndex.x = (_uint)i;
                pVertices[AIWeight.mVertexId].vBlendWeight.x = AIWeight.mWeight;
            }

            else if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.y)
            {
                pVertices[AIWeight.mVertexId].vBlendIndex.y = (_uint)i;
                pVertices[AIWeight.mVertexId].vBlendWeight.y = AIWeight.mWeight;
            }

            else if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.z)
            {
                pVertices[AIWeight.mVertexId].vBlendIndex.z = (_uint)i;
                pVertices[AIWeight.mVertexId].vBlendWeight.z = AIWeight.mWeight;
            }

            else
            {
                pVertices[AIWeight.mVertexId].vBlendIndex.w = (_uint)i;
                pVertices[AIWeight.mVertexId].vBlendWeight.w = AIWeight.mWeight;
            }
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

HRESULT CMesh::Ready_VertexBuffer_For_NonAnim_Binary(const IMPORT_MESH_DESC& Desc, _fmatrix PreTransformMatrix)
{
    return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_Anim_Binary(const CModel* pModel, const IMPORT_MESH_DESC& Desc)
{
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

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const class CModel* pModel, const IMPORT_MESH_DESC& Desc, _fmatrix PreTransformMatrix)
{
    CMesh* pInstance = new CMesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eType, pModel, Desc, PreTransformMatrix)))
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
}
