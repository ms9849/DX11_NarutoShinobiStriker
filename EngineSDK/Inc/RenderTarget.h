#pragma once

/*
렌더 타겟을 그려내기 위한 클래스.
현재 디퓨즈 렌더타겟, 노멀 렌더타겟, 셰이드 렌더타겟을 출력하고 있다.

디퓨즈와 노멀 렌더 타겟을 합성하여 셰이더 단에서 조명 연산을 수행하는, 
기본적인 후처리 셰이딩 단계를 담당하고 있다.
*/

#include "Base.h"

NS_BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() const {
		return m_pRTV;
	}

public:
	HRESULT Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);
	void Clear();
#ifdef _DEBUG
public:
	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	void Render_Debug(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
private:
	ID3D11Texture2D* m_pTexture2D = { nullptr };
	ID3D11RenderTargetView* m_pRTV = { nullptr };
	ID3D11ShaderResourceView* m_pSRV = { nullptr };
	_float4		m_vClearColor = {};

#ifdef _DEBUG
private:
	_float4x4		m_WorldMatrix = {};
#endif

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	virtual void Free() override;
};

NS_END