#pragma once

#include "Base.h"

/*
피킹을 담당하는 클래스.

피킹 된 물체도 가져올 수 있게 해야하나?
*/
NS_BEGIN(Engine)

class CPicking : public CBase
{
	friend class CIMGUI_Manager;
private:
	CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd);
	void Update();
	void Transform_ToLocalSpace(const _float4x4* pWorldMatrixInverse);

	_bool Picking_InWorldSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut);
	_bool Picking_InLocalSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut);

private:
	ID3D11Device* m_pDevice = {};
	ID3D11DeviceContext* m_pContext = {};
	HWND m_hWnd = {};
	_uint m_iWinSizeX{}, m_iWinSizeY{};

public:
	static CPicking* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd);
	virtual void Free() override;
};

NS_END