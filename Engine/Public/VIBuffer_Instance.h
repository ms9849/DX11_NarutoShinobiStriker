#pragma once

#include "VIBuffer.h"

/*
인스턴싱을 위한 버퍼들의 상위 객체.

인스턴싱을 수행하게 되면 2개 이상의 버텍스 버퍼를 가지게 되므로,

추가적인 버텍스 버퍼 하나,
버텍스 버퍼 데스크,
버텍스 버퍼 Initial 데스크,

인스턴싱할 정점의 갯수,
인스턴싱할 정점 하나의 크기 (Stride),
인스턴스 하나 당 인덱스의 갯수 ( 현재 구조에서 인스턴스용 버퍼는 나중에 생성되므로. )

-> 메쉬를 인스턴싱 하려면 ?
-> 모델의 메시를 읽어와야 함.
-> 모델의 메시에 맞는 정점 세팅해주고 똑같이 인스턴스 버텍스 버퍼도 만들어줘야 할거고..
-> 위치 세팅도 해줘야 할 것.
*/

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance abstract : public CVIBuffer
{
public:
	typedef struct tagInstanceDesc
	{
		_uint	iNumInstance = {};
		_float2 vSize = {};
		_float3 vCenter = {};
		_float3 vRange = {};

	} INSTANCE_DESC;
protected:
	CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& rhs);
	virtual ~CVIBuffer_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;

protected:
	ID3D11Buffer*		m_pVBInstance = { nullptr };
	D3D11_BUFFER_DESC	m_InstanceBufferDesc = {};
	D3D11_SUBRESOURCE_DATA m_InstanceInitialDesc = {};
	_uint				m_iInstanceStride = {};
	_uint				m_iNumInstance = {};
	_uint				m_iNumIndexPerInstance = {};

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END 
