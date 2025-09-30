#pragma once

#include "Component.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"


NS_BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;


public:
	virtual HRESULT Initialize_Prototype(COLLIDER eType);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Update(_fmatrix WorldMatrix);

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif
	/* 활성화 되어있는지에 따라 콜리전 매니저에서의 충돌도 제어할 수 있게 한다. */
public:
	_float3 Get_Pos() { return m_pBounding->Get_Pos(); }
	_bool Get_Active() { return m_pBounding->Get_Active();  }
	void Set_Active(_bool bFlag) { m_pBounding->Set_Active(bFlag); }
	
public:
	_bool InterSect(CCollider* pTargetCollider); 

private:
	COLLIDER			m_eType = { COLLIDER::END };
	class CBounding*	m_pBounding = { nullptr };
	_bool				m_isColl = { false };

#ifdef _DEBUG
private:
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType);
	virtual CComponent* Clone(void* pArg) override;	
	virtual void Free() override;
};

NS_END
