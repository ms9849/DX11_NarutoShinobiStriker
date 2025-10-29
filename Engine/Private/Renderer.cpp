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

	/* Target_Outline*/
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Outline"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* 얼룩말 무늬 현상 방지 및, 음수 단위의 깊이도 체크하기 위해 픽셀 포맷을 다르게 세팅 */
	/* Target_Depth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 1.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* Target_Specular */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_Shadow */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shadow"), g_iMaxWidth * m_fShadowFactor, g_iMaxHeight * m_fShadowFactor, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.0f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* Target_StaticShadow */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_StaticShadow"), g_iMaxWidth * 2.f , g_iMaxHeight * 2.f, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.0f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(Ready_DepthStencilView(g_iMaxWidth, g_iMaxHeight)))
		return E_FAIL;

	/* Target_Blur */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_Blur_X */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_X"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_Blur_Small */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_Small"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_Blur_Small_X */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_Small_X"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_Distortion */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Distortion"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_Scene */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Scene"), Viewport.Width, Viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* 게임 오브젝트로부터 뽑아와야하는 디퓨즈 노멀은 MRT_GameObjects로 세팅. */
	/* MRT_GameObjects */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;
	/* MRT_Outline */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Outline"), TEXT("Target_Outline"))))
		return E_FAIL;

	/* MRT_LightAcc */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;

	/* MRT_Scene */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Scene"), TEXT("Target_Scene"))))
		return E_FAIL;

	/////* Specular는 LightAcc에서 쌓아서 계산한다. */
	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
	//	return E_FAIL;

	/* MRT_Shadow */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_Shadow"))))
		return E_FAIL;

	/* MRT_StaticShadow */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_StaticShadow"), TEXT("Target_StaticShadow"))))
		return E_FAIL;

	/* 블러는 렌더타겟 다 따로 둬야한다. */
	/* MRT_Blur */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur"), TEXT("Target_Blur"))))
		return E_FAIL;
	/* MRT_Blur_X */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_X"), TEXT("Target_Blur_X"))))
		return E_FAIL;

	/* MRT_Blur_Small */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_Small"), TEXT("Target_Blur_Small"))))
		return E_FAIL;
	/* MRT_Blur_Small_X */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_Small_X"), TEXT("Target_Blur_Small_X"))))
		return E_FAIL;

	/* MRT_Distortion */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
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
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"), Viewport.Width - 75.f, 75.f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), Viewport.Width - 75.f, 225.f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"), Viewport.Width - 225.f, 75.f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Outline"), Viewport.Width - 225.f, 225.f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shadow"), Viewport.Width - 75.f, 375.f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_StaticShadow"), Viewport.Width - 225.f, 375.f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Blur_Small"), Viewport.Width - 150.f, 150.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Blur_Small_X"), Viewport.Width - 150.f, 450.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Distortion"), Viewport.Width - 150.f, 150.f, 300.f, 300.f)))
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

void CRenderer::Update(_float fTimeDelta)
{
	if (true == m_IsRadialBlur)
	{
		m_fRadialBlurTimeAcc += fTimeDelta;

		if (m_fRadialBlurTimeAcc >= m_fRadialBlurTime)
		{
			m_IsRadialBlur = false;
			m_fRadialBlurTimeAcc = 0.f;
		}
	}

	if (true == m_IsDistortion)
	{
		m_fDistortionTimeAcc += fTimeDelta;

		if (m_fDistortionTimeAcc >= m_fDistortionTime)
		{
			m_IsDistortion = false;
			m_fDistortionTimeAcc = 0.f;
		}
	}
}

void CRenderer::Render()
{
	Render_Priority();
	Render_Shadow();
	Render_NonBlend();
	Render_LightAcc();
	Render_Blur();
	Render_Blur_Small();
	Render_Distortion();
	Render_Combined();
	Render_NonLight();
	Render_Blend();
	Render_UI();
	Render_WorldUI();
	Render_Font();

#ifdef _DEBUG
	Render_Debug();
#endif
}

#ifdef _DEBUG

HRESULT CRenderer::Add_DebugComponent(CComponent* pDebugComponent)
{
	if (nullptr == pDebugComponent)
		return E_FAIL;

	m_DebugComponents.push_back(pDebugComponent);

	Safe_AddRef(pDebugComponent);

	return S_OK;
}

void CRenderer::Set_RadialBlur(_float fTime)
{
	m_fRadialBlurTimeAcc = 0.f;
	m_fRadialBlurTime = fTime;
	m_IsRadialBlur = true;
}

void CRenderer::Set_Distortion(_float fTime)
{
	m_fDistortionTimeAcc = 0.f;
	m_fDistortionTime = fTime;
	m_IsDistortion = true;
}

#endif

void CRenderer::Add_StaticShadow(CGameObject* pGameObject)
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_StaticShadow"), m_pStaticShadowDSV)))
		return;

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)g_iMaxWidth * 2.f;
	ViewPortDesc.Height = (_float)g_iMaxHeight * 2.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	/* 스태틱 섀도우 렌더. */
	if (nullptr != pGameObject)
		pGameObject->Render_Shadow();

	/* 모든 렌더타겟을 비워. */
	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	/* 임시로 하드코딩. 추후 클라이언트에서 받아와야 함. */
	ViewPortDesc.Width = (_float)1280.0f;
	ViewPortDesc.Height = (_float)720.0f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);
}

void CRenderer::Render_Priority()
{
	/* MRT 씬이 아니라 백버퍼에 그려. */
	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::PRIORITY)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::PRIORITY)].clear();
}

void CRenderer::Render_Shadow()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow"), m_pShadowDSV)))
		return;

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)g_iMaxWidth * m_fShadowFactor;
	ViewPortDesc.Height = (_float)g_iMaxHeight * m_fShadowFactor;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);


	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::SHADOW)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_Shadow();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::SHADOW)].clear();

	/* 모든 렌더타겟을 비워. */
	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	/* 임시로 하드코딩. 추후 클라이언트에서 받아와야 함. */
	ViewPortDesc.Width = (_float)1280.0f;
	ViewPortDesc.Height = (_float)720.0f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);
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

	/*
	노말 정보 어디서 가져오는데? 
	-> 클라 셰이더에서..
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

	/* normal 정보 가져왔으면 바로 outline도 세팅하기. */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Outline"))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return;

	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

	m_pShader->Begin(5);

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

/* 이 함수에서 쓰이는 셰이더는 Shader_Deffered, 디퍼드 셰이딩을 위한 셰이더 파일 */
void CRenderer::Render_LightAcc()
{
	/* 후처리 조명 연산을 위한 함수. 앞서 기록된 Target_Normal을 이용하여 조명 연산을 수행한다.*/

	/* MRT_LightAcc (Shade 렌더타겟) 을 세팅. */
	/* -> 우선적으로 조명 연산의 결과가 Shade에 기록된다. */

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
		return;

	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_PipeLine_InverseFloat4x4(D3DTS::VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_PipeLine_InverseFloat4x4(D3DTS::PROJ));
	m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamState(STATE::POSITION), sizeof(_float4));

	/* 셰이더에 Target_Normal 바인딩. */
	/* -> g_NormalTexture라는 이름으로 올라가게 된다. */
	/* -> PS_MAIN_DIRECTIONAL & PS_MAIN_POINT 에서 사용된다. */
	/* 또한 여기서 실질적인 조명 연산이 이루어지게 된다. */

	/* 렌더링이 끝난 Target_Normal의 내용물을 g_NormalTexture에 올려준다.*/
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return;

	/* 화면 전체에 대해 조명 연산이 들어가야하니까 직교투영한 뒤 Bind Resource 세팅 */
	/* 픽셀 셰이더를 수행하기 위해, Bind Resource를 수행한다. */
	// (위에선 이미 Draw가 돌아가기 때문에 픽셀 셰이더가 알아서 돌아감.) 
	m_pVIBuffer->Bind_Resources();

	/* 셰이더에 조명 정보 바인딩. */
	/* 여기서 빛들 쌓아서 계산해. */
 	if (FAILED(m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer)))
		return;

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_Combined()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Scene"))))
		return;

	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_PipeLine_InverseFloat4x4(D3DTS::VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_PipeLine_InverseFloat4x4(D3DTS::PROJ));

	m_pShader->Bind_RawValue("g_IsRadialBlur", &m_IsRadialBlur, sizeof(_bool));
	m_pShader->Bind_RawValue("g_fRadialBlurTime", &m_fRadialBlurTime, sizeof(_float));
	m_pShader->Bind_RawValue("g_fRadialBlurTimeAcc", &m_fRadialBlurTimeAcc, sizeof(_float));

	/* 그림자용 뷰, 투영 행렬 세팅 */
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightViewMatrix", D3DTS::VIEW)))
		return;
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightProjMatrix", D3DTS::PROJ)))
		return;
	/* 기록되어 있던 Diffuse 세팅 */
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return;
	/* 기록되어 있던 Shade (조명 연산의 결과, 명암.) 세팅*/
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
		return;
	///* 아웃라인 텍스쳐도 셰이더에 넘겨줘 */
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Outline"), m_pShader, "g_OutlineTexture")))
		return;

	/* 뎁스와 섀도우도 기록 */
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Shadow"), m_pShader, "g_ShadowTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_StaticShadow"), m_pShader, "g_StaticShadowTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur_X"), m_pShader, "g_BlurXTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur_Small_X"), m_pShader, "g_BlurSmallXTexture")))
		return;

	/* 셰이더에서 이 둘 곱해서 최종적인 계산값 뽑아냄. */
	m_pShader->Begin(3);
	/* 실질적으로 조명 연산이 끝난 NonBlend 객체들 렌더*/
	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	m_pShader->Bind_RawValue("g_IsDistortion", &m_IsDistortion, sizeof(_bool));
	m_pShader->Bind_RawValue("g_fDistortionTime", &m_fDistortionTime, sizeof(_float));
	m_pShader->Bind_RawValue("g_fDistortionTimeAcc", &m_fDistortionTimeAcc, sizeof(_float));

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Scene"), m_pShader, "g_SceneTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Distortion"), m_pShader, "g_DistortionTexture")))
		return;

	m_pShader->Begin(7);
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

void CRenderer::Render_Blur()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur"))))
		return;

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::BLUR)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::BLUR)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_X"))))
		return;

	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

	/* Blur 텍스쳐 바인딩. */
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur"), m_pShader, "g_BlurTexture")))
		return;

	m_pShader->Begin(4);

	m_pVIBuffer->Bind_Resources();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_Blur_Small()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Small"))))
		return;

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::BLUR_SMALL)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::BLUR_SMALL)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Small_X"))))
		return;

	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

	/* Blur 텍스쳐 바인딩. */
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur_Small"), m_pShader, "g_BlurSmallTexture")))
		return;

	m_pShader->Begin(6);

	m_pVIBuffer->Bind_Resources();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

/* 디스토션은 그냥 받아와서 처리만 해주면 된다. */
void CRenderer::Render_Distortion()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Distortion"))))
		return;

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::DISTORTION)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::DISTORTION)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
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

void CRenderer::Render_WorldUI()
{
	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::WORLD_UI)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::WORLD_UI)].clear();
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

HRESULT CRenderer::Ready_DepthStencilView(_uint iSizeX, _uint iSizeY)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D* pDepthStencilTexture = { nullptr } ;
	ID3D11Texture2D* pStaticDepthStencilTexture = { nullptr };
	ID3D11Texture2D* pSceneDepthStencilTexture = { nullptr };
	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	/* 깊이 버퍼의 픽셀은 백버퍼의 픽셀과 갯수가 동일해야만 깊이 텍스트가 가능해진다. */
	/* 픽셀의 수가 다르면 아에 렌더링을 못함. */
	TextureDesc.Width = iSizeX * m_fShadowFactor;
	TextureDesc.Height = iSizeY * m_fShadowFactor;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	/* 동적? 정적?  */
	TextureDesc.Usage = D3D11_USAGE_DEFAULT /* 정적 */;
	/* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
	/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	TextureDesc.Width = iSizeX * 2.f;
	TextureDesc.Height = iSizeY * 2.f;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pStaticDepthStencilTexture)))
		return E_FAIL;

	TextureDesc.Width = 1280.f;
	TextureDesc.Height = 72.0f;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pSceneDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDSV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pStaticDepthStencilTexture, nullptr, &m_pStaticShadowDSV)))
		return E_FAIL;


	if (FAILED(m_pDevice->CreateDepthStencilView(pSceneDepthStencilTexture, nullptr, &m_pSceneDSV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);
	Safe_Release(pStaticDepthStencilTexture);
	Safe_Release(pSceneDepthStencilTexture);

	return S_OK;
}

#ifdef _DEBUG

void CRenderer::Render_Debug()
{
	for (auto& pDebugComponent : m_DebugComponents)
	{
		if (nullptr != pDebugComponent)
			pDebugComponent->Render();

		Safe_Release(pDebugComponent);
	}
	m_DebugComponents.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	/* 렌더타겟을 디버그로 직교투영을 통해 그려라. */
	//if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
	//	return;
	//if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
	//	return;
	//if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Outline"), m_pShader, m_pVIBuffer)))
	//	return;
	//if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Shadow"), m_pShader, m_pVIBuffer)))
	//	return;
	//if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_StaticShadow"), m_pShader, m_pVIBuffer)))
	//	return;
	//if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Blur_Small"), m_pShader, m_pVIBuffer)))
	//	return;
	//if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Blur_Small_X"), m_pShader, m_pVIBuffer)))
	//	return;
	//if (FAILED(m_pGameInstance->Render_RT_Debug(TEXT("MRT_Distortion"), m_pShader, m_pVIBuffer)))
	//	return;
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
	Safe_Release(m_pShadowDSV);
	Safe_Release(m_pStaticShadowDSV);
	Safe_Release(m_pSceneDSV);
}
