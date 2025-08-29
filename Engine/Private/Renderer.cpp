#include "Renderer.h"

#include "UIObject.h"
#include "Font.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
    return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDER eRenderGroup, CGameObject* pRenderObject)
{
	if (nullptr == pRenderObject)
		return E_FAIL;

	m_RenderObjects[ENUM_CLASS(eRenderGroup)].push_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Add_Font(CFont* pRenderFont)
{
	if (nullptr == pRenderFont)
		return E_FAIL;

	m_Fonts.push_back(pRenderFont);

	Safe_AddRef(pRenderFont);

	return S_OK;
}

void CRenderer::Render()
{
	Render_Priority();
	Render_NonBlend();
	Render_Blend();
	Render_UI();
	Render_Font();
}

void CRenderer::Render_Priority()
{
	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::PRIORITY)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::PRIORITY)].clear();
}

void CRenderer::Render_NonBlend()
{
	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::NONBLEND)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::NONBLEND)].clear();
}

void CRenderer::Render_Blend()
{
	m_RenderObjects[ENUM_CLASS(RENDER::BLEND)].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
	{
		return static_cast<CGameObject*>(pSour)->Get_CamDistance() > static_cast<CGameObject*>(pDest)->Get_CamDistance();
	});

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::BLEND)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::BLEND)].clear();
}

void CRenderer::Render_UI()
{
	m_RenderObjects[ENUM_CLASS(RENDER::UI)].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
		{
			return static_cast<CUIObject*>(pSour)->Get_ZOrder() > static_cast<CUIObject*>(pDest)->Get_ZOrder();
		});

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::UI)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::UI)].clear();
}

void CRenderer::Render_Font()
{

	/* 블렌딩 / 스텐실뎁스 / 래스터라이즈 스테이트 저장해야됨 */
	ID3D11BlendState*		 pPreBlendState = {};
	_float					 PreBlendFactor[4] = {};
	_uint					 PreBlendSampleMask = {};
	m_pContext->OMGetBlendState(&pPreBlendState, PreBlendFactor, &PreBlendSampleMask);

	ID3D11DepthStencilState* pPreDepthStencilState = {};
	_uint					 PreDepthStencilRef = {};
	m_pContext->OMGetDepthStencilState(&pPreDepthStencilState, &PreDepthStencilRef);

	ID3D11RasterizerState*	 pPreRasterizeState = {};
	m_pContext->RSGetState(&pPreRasterizeState);

	for (auto& pFont : m_Fonts)
	{
		if (nullptr != pFont)
			pFont->DrawFont();

		Safe_Release(pFont);
	}

	m_pContext->OMSetBlendState(pPreBlendState, PreBlendFactor, PreBlendSampleMask);
	Safe_Release(pPreBlendState);

	m_pContext->OMSetDepthStencilState(pPreDepthStencilState, PreDepthStencilRef);
	Safe_Release(pPreDepthStencilState);

	m_pContext->RSSetState(pPreRasterizeState);
	Safe_Release(pPreRasterizeState);

	m_Fonts.clear();
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderer::Free()
{
    __super::Free();

	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pRenderObject : RenderObjects)
			Safe_Release(pRenderObject);
		RenderObjects.clear();
	}

	for (auto& pFont : m_Fonts)
		Safe_Release(pFont);

	m_Fonts.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);


}
