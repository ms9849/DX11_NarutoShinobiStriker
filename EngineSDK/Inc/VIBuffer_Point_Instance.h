#pragma once

#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance : public CVIBuffer_Instance
{
public:
	typedef struct tagPointInstanceDesc final : public CVIBuffer_Instance::INSTANCE_DESC
	{
		/* 파티클 시스템 하나당 사용되는 변수 */
		_bool			isLoop;

		/* 중심으로부터 얼마나 떨어져있는지 */
		_float3			vPivot;
		_float2			vSpeed;

		/* 인스턴스 하나당 사용되는 변수 */
		_float4			vColor;
		_float3			vRotation;
		_float2			vLifeTime;

	}POINT_INSTANCE_DESC;

private:
	CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pInstanceDesc) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;

public:
	_bool IsAllDead() { return m_IsDead; }

public:
	virtual void Drop(_float fTimeDelta);
	virtual void Explosion(_float fTimeDelta);

private:
	VTX_INSTANCE_PARTICLE* m_pInstanceVertices = { nullptr };

	_float3 m_vPivot = {};
	_float* m_pSpeeds = { nullptr };
	_bool	m_IsLoop = { false };
	_bool   m_IsDead = { false };

public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pInstanceDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END