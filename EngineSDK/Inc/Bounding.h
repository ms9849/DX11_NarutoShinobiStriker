#pragma once

#include "Base.h"

/*
DXTK에 있는 Bounding & BoundingOriendted, Bounding Sphere 를 이용한 
충돌 구현.

각 타입에 초기 세팅값 (Size, Angle, Radius 등)만 던져주면 알아서 세팅된다.

충돌체 세팅이 끝났다면 -> 각 Bounding Box에 세팅된 Update 함수를 통해 월드행렬을 세팅해준다.

충돌 처리는 DX를 통해 제공되는 InterSect 함수를 이용하여 수행한다.
*/
NS_BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	typedef struct tagBoundingDesc
	{
		_float3			vCenter;
		_bool			isActive = { true };
	}BOUNDING_DESC;

public:
	_bool Get_Active() { return m_isActive; }
	void Set_Active(_bool bFlag) { m_isActive = bFlag; }

protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding() = default;

public:
	HRESULT Initialize();
	virtual void Update(_fmatrix WorldMatrix) = 0;
	virtual _bool Intersect(COLLIDER eType, class CBounding* pTarget) = 0;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) = 0;
#endif

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	_bool					m_isActive = { true };

public:
	virtual void Free();
};

NS_END