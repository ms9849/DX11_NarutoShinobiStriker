#include "VIBuffer_Point_Instance.h"

#include "GameInstance.h"

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype)
    : CVIBuffer_Instance{ Prototype }
    , m_pInstanceVertices{ Prototype.m_pInstanceVertices }
    , m_pSpeeds{ Prototype.m_pSpeeds }
    , m_IsLoop{ Prototype.m_IsLoop }
{
}

HRESULT CVIBuffer_Point_Instance::Initialize_Prototype(const INSTANCE_DESC* pInstanceDesc)
{
	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOS);

	m_iNumIndices = 0;
	m_iIndexStride = 0;


	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEX_BUFFER
	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.StructureByteStride = m_iVertexStride;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	m_pVertexPositions[0] = pVertices[0].vPosition = _float3(0.0f, 0.0f, 0.f);

	D3D11_SUBRESOURCE_DATA	InitialVBData{};
	InitialVBData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
/* 
Point 형태이므로 인스턴스는 사용하지 않는다. 
대신 기하 셰이더 단에서 처리 해줌.
*/
#pragma endregion

#pragma region INSTANCE_BUFFER
	const POINT_INSTANCE_DESC* pDesc = static_cast<const POINT_INSTANCE_DESC*>(pInstanceDesc);
	m_vPivot = pDesc->vPivot;
	m_IsLoop = pDesc->isLoop;
	m_iNumInstance = pDesc->iNumInstance;
	m_iInstanceStride = sizeof(VTX_INSTANCE_PARTICLE);
	m_iNumIndexPerInstance = 6;

	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;

	m_pInstanceVertices = new VTX_INSTANCE_PARTICLE[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTX_INSTANCE_PARTICLE) * m_iNumInstance);

	m_pSpeeds = new _float[m_iNumInstance];
	ZeroMemory(m_pSpeeds, sizeof(_float) * m_iNumInstance);


	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float			fScale = m_pGameInstance->Random(pDesc->vSize.x, pDesc->vSize.y);

		m_pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
		m_pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
		m_pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
		m_pInstanceVertices[i].vTranslation = _float4(
			m_pGameInstance->Random(pDesc->vCenter.x - pDesc->vRange.x * 0.5f, pDesc->vCenter.x + pDesc->vRange.x * 0.5f),
			m_pGameInstance->Random(pDesc->vCenter.y - pDesc->vRange.y * 0.5f, pDesc->vCenter.y + pDesc->vRange.y * 0.5f),
			m_pGameInstance->Random(pDesc->vCenter.z - pDesc->vRange.z * 0.5f, pDesc->vCenter.z + pDesc->vRange.z * 0.5f),
			1.f);

		m_pInstanceVertices[i].vLifeTime = _float2(0.0f, m_pGameInstance->Random(pDesc->vLifeTime.x, pDesc->vLifeTime.y));



		m_pSpeeds[i] = m_pGameInstance->Random(pDesc->vSpeed.x, pDesc->vSpeed.y);
	}

	m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Initialize(void* pArg)
{
	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialDesc, &m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Bind_Resources()
{
	ID3D11Buffer* VertexBuffers[] = {
		   m_pVB,
		   m_pVBInstance,
	};

	_uint		VertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride,
	};

	_uint		Offsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, VertexBuffers, VertexStrides, Offsets);

	m_pContext->IASetPrimitiveTopology(m_ePrimitive);

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Render()
{
	m_pContext->DrawInstanced(1, m_iNumInstance, 0, 0);

	return S_OK;
}

void CVIBuffer_Point_Instance::Drop(_float fTimeDelta)
{
	m_IsDead = true;

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTX_INSTANCE_PARTICLE* pVertices = static_cast<VTX_INSTANCE_PARTICLE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vTranslation.y -= m_pSpeeds[i] * fTimeDelta;
		pVertices[i].vLifeTime.x += fTimeDelta;

		if (true == m_IsLoop &&
			pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
		{
			pVertices[i].vLifeTime.x = 0.f;
			pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
		}
		else
		{
			if (false == m_IsLoop && pVertices[i].vLifeTime.x < pVertices[i].vLifeTime.y)
				m_IsDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Point_Instance::Spread(_float fTimeDelta)
{
	m_IsDead = true;

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTX_INSTANCE_PARTICLE* pVertices = static_cast<VTX_INSTANCE_PARTICLE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		/*pVertices[i].vTranslation.y -= m_pSpeeds[i] * fTimeDelta;*/
		_vector		vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivot), 0.f);

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);

		/* LifeTime의 X는 현재 나이, LifeTime의 y는 총 수명 */
		pVertices[i].vLifeTime.x += fTimeDelta;

		if (true == m_IsLoop &&
			pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
		{
			pVertices[i].vLifeTime.x = 0.f;
			pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
		}
		else
		{
			if (false == m_IsLoop && pVertices[i].vLifeTime.x < pVertices[i].vLifeTime.y)
				m_IsDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Point_Instance::Explosion(_float fTimeDelta)
{
	m_IsDead = true;

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTX_INSTANCE_PARTICLE* pVertices = static_cast<VTX_INSTANCE_PARTICLE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		/*pVertices[i].vTranslation.y -= m_pSpeeds[i] * fTimeDelta;*/
		_vector		vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivot), 0.f);

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);

		pVertices[i].vLifeTime.x += fTimeDelta;

		if (true == m_IsLoop &&
			pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
		{
			pVertices[i].vLifeTime.x = 0.f;
			pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
		}
		else
		{
			if (pVertices[i].vLifeTime.x < pVertices[i].vLifeTime.y)
				m_IsDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

CVIBuffer_Point_Instance* CVIBuffer_Point_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pInstanceDesc)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pInstanceDesc)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Point_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Point_Instance::Clone(void* pArg)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Point_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pInstanceVertices);
		Safe_Delete_Array(m_pSpeeds);
	}
}
