#include "Trail.h"

#include "GameInstance.h"

CTrail::CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CTrail::CTrail(const CTrail& rhs)
    : CGameObject{ rhs } ,
	m_eType { rhs.m_eType },
    m_pIB { rhs.m_pIB },
    m_iNumVertices { rhs.m_iNumVertices },
    m_iNumIndices { rhs.m_iNumIndices }
{
	Safe_AddRef(m_pIB);
}

HRESULT CTrail::Initialize_Prototype(void* pArg)
{
	PROTOTYPE_TRAIL_DESC* pDesc = static_cast<PROTOTYPE_TRAIL_DESC*>(pArg);

	m_eType = pDesc->eType;
	m_iNumVertices = pDesc->iNumVertices;
	m_iNumIndices = ((m_iNumVertices / 2) - 1) * 6;

#pragma region IDX_BUFFER
	D3D11_BUFFER_DESC IBDesc{};
	IBDesc.ByteWidth = sizeof(_uint) * m_iNumIndices;
	IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = sizeof(_uint);


	_uint* pIndices = new _uint[m_iNumIndices]{};
	_uint iIndex = 0;
	for (_uint iStartIndex = 0; iStartIndex < (m_iNumVertices / 2) - 1; ++iStartIndex)
	{
		_uint i0 = iStartIndex * 2;
		_uint i1 = i0 + 1;
		_uint i2 = i0 + 2;
		_uint i3 = i0 + 3;

		pIndices[iIndex++] = i0;
		pIndices[iIndex++] = i1;
		pIndices[iIndex++] = i2;

		pIndices[iIndex++] = i2;
		pIndices[iIndex++] = i1;
		pIndices[iIndex++] = i3;
	}

	D3D11_SUBRESOURCE_DATA InitialIBData{};
	InitialIBData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitialIBData, &m_pIB))) {
		return E_FAIL;
	}
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CTrail::Initialize(void* pArg)
{
	TRAIL_DESC* pDesc = static_cast<TRAIL_DESC*>(pArg);
	XMStoreFloat4(&m_vHigh, XMLoadFloat4(&pDesc->vHighPosition));
	XMStoreFloat4(&m_vLow, XMLoadFloat4(&pDesc->vLowPosition));
	m_vFootTrailColor = pDesc->vFootTrailColor;

	m_iNumPresent = 0;
	m_iEndIndex = 0;

	if (FAILED(Ready_Components(pDesc->strTrailTextureTag)))
		return E_FAIL;

#pragma region VTX_BUFFER
	D3D11_BUFFER_DESC VBDesc{};
	VBDesc.ByteWidth = sizeof(VTXPOSTEX) * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = sizeof(VTXPOSTEX);

	m_pVTXPOSTEXs = new VTXPOSTEX[m_iNumVertices];
	memset(m_pVTXPOSTEXs, 0, sizeof(VTXPOSTEX) * m_iNumVertices);

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, nullptr, &m_pVB))) {
		return E_FAIL;
	}

#pragma endregion


	return S_OK;
}

void CTrail::Priority_Update(_float fTimeDelta)
{
}

void CTrail::Update(_float fTimeDelta)
{
}

void CTrail::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::BLUR, this);
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

void CTrail::Update_Trail(_fmatrix matCurrentWorld, _bool IsMakeTrail)
{
	_float3 vCurHighPos, vCurLowPos;

	if (!IsMakeTrail)
	{
		if (m_iNumPresent > 2)
		{
			m_iNumPresent -= 2;
			memmove(m_pVTXPOSTEXs, m_pVTXPOSTEXs + 2, sizeof(VTXPOSTEX) * m_iNumPresent);
		}
		return;
	}

	XMStoreFloat3(&vCurHighPos, XMVector3TransformCoord(XMLoadFloat4(&m_vHigh), matCurrentWorld));
	XMStoreFloat3(&vCurLowPos, XMVector3TransformCoord(XMLoadFloat4(&m_vLow), matCurrentWorld));

	// 이전 점 업데이트
	m_vPreHighPositions[0] = m_vPreHighPositions[1];
	m_vPreHighPositions[1] = m_vPreHighPositions[2];
	m_vPreHighPositions[2] = vCurHighPos;

	m_vPreLowPositions[0] = m_vPreLowPositions[1];
	m_vPreLowPositions[1] = m_vPreLowPositions[2];
	m_vPreLowPositions[2] = vCurLowPos;

	// 시작 시 이전 점이 없으면 현재 점으로 초기화
	if (m_iNumPresent == 0)
	{
		m_vPreHighPositions[0] = vCurHighPos;
		m_vPreHighPositions[1] = vCurHighPos;
		m_vPreLowPositions[0] = vCurLowPos;
		m_vPreLowPositions[1] = vCurLowPos;
	}

	_uint InterpSteps = 8; // 보간점 수
	for (_uint i = 0; i < InterpSteps; ++i)
	{
		_float t = (_float)i / (_float)(InterpSteps - 1);

		// Catmull-Rom P0,P1,P2,P3 설정
		_vector P0_High = XMVectorSetW(XMLoadFloat3(&m_vPreHighPositions[0]), 1.f);
		_vector P1_High = XMVectorSetW(XMLoadFloat3(&m_vPreHighPositions[0]), 1.f);
		_vector P2_High = XMVectorSetW(XMLoadFloat3(&m_vPreHighPositions[1]), 1.f);
		_vector P3_High = XMVectorSetW(XMLoadFloat3(&m_vPreHighPositions[2]), 1.f);

		_vector P0_Low = XMVectorSetW(XMLoadFloat3(&m_vPreLowPositions[0]), 1.f);
		_vector P1_Low = XMVectorSetW(XMLoadFloat3(&m_vPreLowPositions[0]), 1.f);
		_vector P2_Low = XMVectorSetW(XMLoadFloat3(&m_vPreLowPositions[1]), 1.f);
		_vector P3_Low = XMVectorSetW(XMLoadFloat3(&m_vPreLowPositions[2]), 1.f);

		_vector HighInterp = XMVectorCatmullRom(P0_High, P1_High, P2_High, P3_High, t);
		_vector LowInterp = XMVectorCatmullRom(P0_Low, P1_Low, P2_Low, P3_Low, t);

		// 정점 버퍼 오버플로우 체크
		if (m_iNumPresent + 2 >= m_iNumVertices)
		{
			m_iNumPresent -= 2;
			memmove(m_pVTXPOSTEXs, m_pVTXPOSTEXs + 2, sizeof(VTXPOSTEX) * m_iNumPresent);
		}

		XMStoreFloat3(&m_pVTXPOSTEXs[m_iNumPresent + 1].vPosition, HighInterp);
		XMStoreFloat3(&m_pVTXPOSTEXs[m_iNumPresent].vPosition, LowInterp);
		m_iNumPresent += 2;
	}

	// UV 매핑
	_uint iNumPairs = m_iNumPresent >> 1;
	for (_uint i = 0; i < iNumPairs; ++i)
	{
		_float u = (_float)i / (_float)(iNumPairs - 1);
		_uint iLow = i << 1;
		_uint iHigh = iLow + 1;
		m_pVTXPOSTEXs[iHigh].vTexCoord = { u, 0.f };
		m_pVTXPOSTEXs[iLow].vTexCoord = { u, 1.f };
	}

	D3D11_MAPPED_SUBRESOURCE SubResource{};
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	memcpy(SubResource.pData, m_pVTXPOSTEXs, sizeof(VTXPOSTEX) * m_iNumPresent);
	m_pContext->Unmap(m_pVB, 0);
}


void CTrail::Set_CombinedWorldMatrix(_fmatrix TargetWorldMatrix)
{
	XMStoreFloat4x4(&m_TargetWorldMatrix, TargetWorldMatrix);
}

HRESULT CTrail::Render()
{
	if (true == IsRenderable()) 
		return S_OK; 

	_float4x4 IdentityMatrix = {};
	XMStoreFloat4x4(&IdentityMatrix, XMMatrixIdentity());

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vFootTrailColor", &m_vFootTrailColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &IdentityMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (TRAIL_TYPE::SWORD == m_eType)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX_IDX::TRAIL))))
			return E_FAIL;
	}
	else if (TRAIL_TYPE::FOOT == m_eType)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX_IDX::FOOT_TRAIL))))
			return E_FAIL;
	}

	ID3D11Buffer* VertexBuffers[] = { m_pVB };
	_uint		  VertexStrides[] = { sizeof(VTXPOSTEX) };
	_uint		  Offsets[] = { 0 };

	m_pContext->IASetVertexBuffers(0, 1, VertexBuffers, VertexStrides, Offsets);
	m_pContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);
	m_pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/* 사각형 갯수 */
	_uint iNumPair = m_iNumPresent >> 1;

	/* -1 하고 6 곱하면 인덱스 갯수 */
	_uint iActiveIndices = (iNumPair - 1) * 6;

	/* 마지막 삼각형 하나는 출력 X  */
	m_pContext->DrawIndexed(iActiveIndices , 0, 0);
	return S_OK;
}

HRESULT CTrail::Ready_Components(const _wstring& strTextureTag)
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), strTextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

_bool CTrail::IsRenderable()
{
	return (m_iNumPresent < 4);
}

CTrail* CTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID, void* pArg)
{
	CTrail* pInstance = new CTrail(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX("Failed to Created : CTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrail::Clone(void* pArg)
{
	CTrail* pInstance = new CTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pVTXPOSTEXs);

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
