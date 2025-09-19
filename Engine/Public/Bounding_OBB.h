#pragma once

#include "Bounding.h"

NS_BEGIN(Engine)

class CBounding_OBB final : public CBounding
{
public:
	/* 사이즈 + 각도까지 받아와야 한다. */
	typedef struct tagBoundingOBBDesc : public CBounding::BOUNDING_DESC
	{
		_float3			vSize;
		_float3			vAngles;
	} BOUNDING_OBB_DESC;

private:
	CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_OBB() = default;

public:
	virtual _float3 Get_Pos() override {
		return m_pDesc->Center;
	}


public:
	BoundingOrientedBox* Get_Desc() const {
		return m_pDesc;
	}

public:
	HRESULT Initialize(const void* pArg);
	virtual void Update(_fmatrix WorldMatrix) override;
	/* 상대방의 타입과 콜라이더 그 자체를 받아와서 세팅. */
	virtual _bool Intersect(COLLIDER eType, class CBounding* pTarget) override;

#ifdef _DEBUG
	/* 색상과 미리 만들어진 정점 & 인덱스 정보를 가져와서 세팅. */
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) override;
#endif
private:
	BoundingOrientedBox* m_pOriginalDesc = { nullptr };
	BoundingOrientedBox* m_pDesc = { nullptr };

	
public:
	static CBounding_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const void* pDesc);
	virtual void Free() override;
};

NS_END
