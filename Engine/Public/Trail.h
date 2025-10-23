#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTrail final : public CComponent
{
public:
	typedef struct tagTrail {
		_float4 vHighPosition = {};
		_float4 vLowPosition = {};
	} TRAIL_DESC;
private:
	CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail(const CTrail& rhs);
	virtual ~CTrail() = default;

public:
	_bool IsRenderable();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	void Update_Trail(_fmatrix matCurrentWorld, _bool bMakeTrail = false);
	virtual HRESULT Render() override;

private:
	ID3D11Buffer* m_pVB = { nullptr };
	ID3D11Buffer* m_pIB = { nullptr };
	VTXPOSTEX* m_pVTXPOSTEXs = {};
	_float4			m_vHigh = {};
	_float4			m_vLow = {};
	_uint			m_iNumPresent = {};
	_uint			m_iEndIndex = {};
	_uint			m_iNumVertices = {};
	_uint			m_iNumIndices = {};

public:
	static CTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
