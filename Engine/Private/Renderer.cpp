#include "Renderer.h"

#include "GameInstance.h"
#include "UIObject.h"
#include "Font.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CRenderer::Initialize()
{
	_uint		iNumViewports = { 1 };

	D3D11_VIEWPORT		Viewport{};
	m_pContext->RSGetViewports(&iNumViewports, &Viewport);

	/* 후처리 쉐이딩을 위한 렌더타겟들을 준비하낟. */
	/* Target_Diffuse */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_Normal */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* Target_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* 게임 오브젝트로부터 뽑아와야하는 디퓨즈 노멀은 MRT_GameObjects로 세팅. */
	/* MRT_GameObjects */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;

	/* MRT_LightAcc */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(Viewport.Width, Viewport.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f));

#ifdef _DEBUG
	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"), 640.f, 360.f, 1280.f, 720.f)))
	//	return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"), 75.f, 75.f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), 75.f, 225.f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"), 225.f, 75.f, 150.f, 150.f)))
		return E_FAIL;
#endif


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
	Render_LightAcc();
	Render_Combined();
	Render_NonLight();
	Render_Blend();
	Render_UI();
	Render_Font();

#ifdef _DEBUG
	Render_Debug();
#endif
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
	/* Diffuse + Normal 렌더타겟 세팅*/
	//-> 0번 렌더타겟에 Diffuse, 1번 렌더 타겟에 Normal이 들어가게 된다.

	/*
	기존에는 백버퍼에 바로 그렸다면, 앞으로는 셰이더에서 Diffuse와 Normal을 기록한뒤,
	Normal과 조명간의 연산을 통해 명암을 뽑아내어 MRT_LightAcc에 그려낸뒤, 
	MRT_GameObject와 MRT_LightAcc을 합성하여 Shade를 뽑아내는 식으로 수행할 것.
	*/
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return;

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::NONBLEND)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::NONBLEND)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_LightAcc()
{
	/* 후처리 조명 연산을 위한 함수. 앞서 기록된 Target_Normal을 이용하여 조명 연산을 수행한다.*/

	/* MRT_LightAcc (Shade 렌더타겟) 을 세팅. */
	/* -> 우선적으로 조명 연산의 결과가 Shade에 기록된다. */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
		return;

	/* 이 함수에서 쓰이는 셰이더는 Shader_Deffered, 디퍼드 셰이딩을 위한 셰이더 파일 */
	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

	/* 셰이더에 Target_Normal 바인딩. */
	/* -> g_NormalTexture라는 이름으로 올라가게 된다. */
	/* -> PS_MAIN_DIRECTIONAL 에서 사용된다. */
	/* 또한 여기서 실질적인 조명 연산이 이루어지게 된다. */
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return;

	/* 화면 전체에 대해 조명 연산이 들어가야하니까 직교투영한 뒤 Bind Resource 세팅 */
	/* 픽셀 셰이더를 수행하기 위해, Bind Resource를 수행한다. */
	// (위에선 이미 Draw가 돌아가기 때문에 픽셀 셰이더가 알아서 돌아감.) 
	m_pVIBuffer->Bind_Resources();

	/* 셰이더에 조명 정보 바인딩. */
	if (FAILED(m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer)))
		return;

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_Combined()
{
	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

	/* 기록되어 있던 Diffuse 세팅 */
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return;
	/* 기록되어 있던 Shade (조명 연산의 결과, 명암.) 세팅*/
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return;

	/* 셰이더에서 이 둘 곱해서 최종적인 계산값 뽑아냄. */
	m_pShader->Begin(3);

	/* 실질적으로 조명 연산이 끝난 NonBlend 객체들 렌더*/
	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();
}

void CRenderer::Render_NonLight()
{
	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::NONLIGHT)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::NONLIGHT)].clear();
}

void CRenderer::Render_Blend()
{
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
	for (auto& pFont : m_Fonts)
	{
		if (nullptr != pFont)
			pFont->DrawFont();

		Safe_Release(pFont);
	}

	m_Fonts.clear();
}

#ifdef _DEBUG

void CRenderer::Render_Debug()
{
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	/* 렌더타겟을 디버그로 직교투영을 통해 그려라. */
	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
		return;
	if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
		return;
}

#endif

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
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pGameInstance);
}
