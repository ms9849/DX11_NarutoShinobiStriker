#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CPipeLine final : public CBase
{
private:
	CPipeLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPipeLine() = default;

public:
	HRESULT Initialize();
	void Update();

public:
	const _float4x4& Get_ViewMatrix();
	const _float4x4& Get_CameraWorldMatrix();
	const _float4x4& Get_ProjMatrix();
	void Set_CameraWorldMatrix(const _float4x4& CameraWorldMatrix);
	void Set_ProjMatrix(const _float4x4& ProjMatrix);
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	_float4x4 m_ViewMatrix = {};
	_float4x4 m_ProjMatrix = {};

	/* 뷰 행렬의 역행렬. 하는김에 들고 있기 */
	_float4x4 m_CameraWorldMatrix = {};
public:
	static CPipeLine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
