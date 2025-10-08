#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CFont final : public CComponent
{
private:
	CFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFont(const CFont& rhs);
	virtual ~CFont() = default;

public:
	HRESULT Initialize_Prototype(const _tchar* pFontFilePath);
	HRESULT Initialize(void* pArg);
	HRESULT Bind_Resources(const _tchar* pText, const _float2& vPosition, _bool IsAlign = false,
		_float fScale = 1.f, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRotation = 0.f,
		const _float2& vOrigin = _float2(0.f, 0.f));

	HRESULT DrawFont();

private:
	SpriteFont* m_pFont = { nullptr };
	SpriteBatch* m_pBatch = { nullptr };

	ID3D11SamplerState* m_pSampleState = { nullptr };
	ID3D11BlendState* m_pBlendState = { nullptr };

	_wstring m_Text = {};
	_float2 m_vPosition = {};
	_float  m_fScale = {};
	_vector m_vColor = {};
	_float  m_fRotation = {};
	_float2 m_vOrigin = {};
	_float  m_fWinSizeX{}, m_fWinSizeY{};

public:
	static CFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
