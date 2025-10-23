#include "Trail.h"

#include "GameInstance.h"

CTrail::CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CTrail::CTrail(const CTrail& rhs)
    : CGameObject{ rhs } ,
    m_pIB { rhs.m_pIB },
    m_iNumVertices { rhs.m_iNumVertices },
    m_iNumIndices { rhs.m_iNumIndices }
{
	Safe_AddRef(m_pIB);
}

HRESULT CTrail::Initialize_Prototype()
{
	m_iNumVertices = 36;
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
	if (FAILED(Ready_Components()))
		return E_FAIL;

#pragma region VTX_BUFFER
	D3D11_BUFFER_DESC VBDesc{};
	VBDesc.ByteWidth = sizeof(VTXPOSTEX) * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = sizeof(VTXPOSTEX);

	m_pVTXPOSTEXs = new VTXPOSTEX[m_iNumVertices]{};
	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, nullptr, &m_pVB))) {
		return E_FAIL;
	}
#pragma endregion

	TRAIL_DESC* pDesc = static_cast<TRAIL_DESC*>(pArg);
	XMStoreFloat4(&m_vHigh, XMLoadFloat4(&pDesc->vHighPosition));
	XMStoreFloat4(&m_vLow, XMLoadFloat4(&pDesc->vLowPosition));

	m_iNumPresent = 0;
	m_iEndIndex = 0;
	memset(m_pVTXPOSTEXs, 0, sizeof(VTXPOSTEX) * m_iNumVertices);

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
	if(true == m_IsBlur)
		m_pGameInstance->Add_RenderGroup(RENDER::BLUR, this);
	else
		m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

//void CTrail::Update_Trail(_fmatrix matCurrentWorld, _bool IsMakeTrail)
//{
//	_float3 vCurHightPos, vCurLowPos;
//
//	if (false == IsMakeTrail)
//	{
//		if (0 != m_iNumPresent) {
//			m_iNumPresent -= 2;
//			memmove(m_pVTXPOSTEXs, m_pVTXPOSTEXs + 2, sizeof(VTXPOSTEX) * m_iNumPresent);
//		}
//
//		return;
//	}
//
//	XMStoreFloat3(&vCurHightPos, XMVector3TransformCoord(XMLoadFloat4(&m_vHigh), matCurrentWorld));
//	XMStoreFloat3(&vCurLowPos, XMVector3TransformCoord(XMLoadFloat4(&m_vLow), matCurrentWorld));
//
//	m_vPreHighPositions[0] = m_vPreHighPositions[1];
//	m_vPreHighPositions[1] = m_vPreHighPositions[2];
//	XMStoreFloat3(&m_vPreHighPositions[2], XMLoadFloat3(&vCurHightPos));
//
//	m_vPreLowPositions[0] = m_vPreLowPositions[1];
//	m_vPreLowPositions[1] = m_vPreLowPositions[2];
//	XMStoreFloat3(&m_vPreLowPositions[2], XMLoadFloat3(&vCurLowPos));
//
//	/* 
//	스플라인이 곡선 형태로 보간해주는건 맞지만 
//	결국 정점 갯수가 늘어나야 자연스러움.
//
//	XMVectorCatmullRom에 들어가는 P0, P3은 예측을 위한 값.
//	P1과 P2가 실제 보간에 사용되는 값.
//
//	해당 함수로 생성되는 값은 보간이 없더라도 
//	실제 함수를 통과하는 값임이 보장됨.
//	*/
//	for (_uint i = 0; i < 4; ++i)
//	{
//		_float fValue = (_float)i / 3;
//
//		_vector vFirstHighPoint{}, vSecondHighPoint{}, vThirdHighPoint{};
//		_vector vFirstLowPoint{}, vSecondLowPoint{}, vThirdLowPoint{};
//
//		vFirstHighPoint = XMVectorSetW(XMLoadFloat3(&m_vPreHighPositions[0]), 1.f);
//		vSecondHighPoint = XMVectorSetW(XMLoadFloat3(&m_vPreHighPositions[1]), 1.f);
//		vThirdHighPoint = XMVectorSetW(XMLoadFloat3(&m_vPreHighPositions[2]), 1.f);
//
//		vFirstLowPoint = XMVectorSetW(XMLoadFloat3(&m_vPreLowPositions[0]), 1.f);
//		vSecondLowPoint = XMVectorSetW(XMLoadFloat3(&m_vPreLowPositions[1]), 1.f);
//		vThirdLowPoint = XMVectorSetW(XMLoadFloat3(&m_vPreLowPositions[2]), 1.f);
//
//		_vector vHighInterp = XMVectorCatmullRom(vFirstHighPoint, vSecondHighPoint, vThirdHighPoint, vThirdHighPoint + (vThirdHighPoint - vFirstHighPoint), fValue);
//		_vector vLowInterp = XMVectorCatmullRom(vFirstLowPoint, vSecondLowPoint, vThirdLowPoint, vThirdLowPoint + (vThirdLowPoint - vSecondHighPoint), fValue);
//
//		// 정점 버퍼가 넘치면 memmove로 빠르게 옮겨주기
//		if (m_iNumPresent + 2 >= m_iNumVertices)
//		{
//			m_iNumPresent -= 2;
//			memmove(m_pVTXPOSTEXs, m_pVTXPOSTEXs + 2, sizeof(VTXPOSTEX) * m_iNumPresent);
//		}
//
//		XMStoreFloat3(&m_pVTXPOSTEXs[m_iNumPresent + 1].vPosition, vHighInterp);
//		XMStoreFloat3(&m_pVTXPOSTEXs[m_iNumPresent].vPosition, vLowInterp);
//		m_iNumPresent += 2;
//	}
//
//	_uint iNumActivatedPairs = m_iNumPresent >> 1;
//	if (iNumActivatedPairs < 2) 
//		return; // 시작한지 얼마 안돼서 정점 갯수가 4 미만인 경우 얼리 리턴
//
//	// UV 매핑
//	for (_uint iIndex = 0; iIndex < iNumActivatedPairs; ++iIndex) {
//		_float u = (_float)iIndex / (_float)(iNumActivatedPairs - 1);
//		// 전체 UV에서 현재 노트가 위치한 u구하기 ( uv 늘이기 )
//
//		_uint iIndexLow = iIndex << 1;
//		_uint iIndexHigh = iIndexLow + 1;
//
//		m_pVTXPOSTEXs[iIndexHigh].vTexCoord = { u, 0.f };
//		m_pVTXPOSTEXs[iIndexLow].vTexCoord = { u, 1.f };
//	}
//
//	D3D11_MAPPED_SUBRESOURCE SubResource{};
//	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
//
//	VTXPOSTEX* pVertices = static_cast<VTXPOSTEX*>(SubResource.pData);
//	for (_uint i = 0; i < m_iNumPresent; ++i) {
//		pVertices[i] = m_pVTXPOSTEXs[i];
//	}
//	m_pContext->Unmap(m_pVB, 0);
//}

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

	const _uint InterpSteps = 8; // 보간점 수
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

	// GPU에 전송
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

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &IdentityMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX_IDX::TRAIL))))
		return E_FAIL;

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

HRESULT CTrail::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SwordTrail"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

_bool CTrail::IsRenderable()
{
	return (m_iNumPresent < 4);
}

CTrail* CTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CTrail* pInstance = new CTrail(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
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
