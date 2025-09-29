#include "RenderTarget.h"

#include "Shader.h"
#include "VIBuffer_Rect.h"

CRenderTarget::CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    /* 텍스쳐 Desc 생성 */
    D3D11_TEXTURE2D_DESC            TextureDesc{};

    TextureDesc.Width = iSizeX;
    TextureDesc.Height = iSizeY;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = ePixelFormat;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    /* 
    Device를 이용하여 현재 화면의 크기와 같은 텍스쳐를 생성하고,
    해당 텍스쳐를 기반으로 렌더 타겟 뷰와 셰이더 리소스 뷰를 생성한다. 
    */
    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
        return E_FAIL;

    m_vClearColor = vClearColor;

    return S_OK;
}

HRESULT CRenderTarget::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
    /* 현재 들고 있는 셰이더 리소스 뷰를 바인딩한다. */
    if (FAILED(pShader->Bind_SRV(pConstantName, m_pSRV)))
        return E_FAIL;

    return S_OK;
}

void CRenderTarget::Clear()
{
    m_pContext->ClearRenderTargetView(m_pRTV, reinterpret_cast<_float*>(&m_vClearColor));
}

#ifdef _DEBUG

HRESULT CRenderTarget::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
    /* 렌더 타겟의 월드행렬 세팅. */
    _uint		iNumViewports = { 1 };

    D3D11_VIEWPORT		Viewport{};
    m_pContext->RSGetViewports(&iNumViewports, &Viewport);


    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
    m_WorldMatrix._11 = fSizeX;
    m_WorldMatrix._22 = fSizeY;
    m_WorldMatrix._41 = fX - Viewport.Width * 0.5f;
    m_WorldMatrix._42 = -fY + Viewport.Height * 0.5f;

    return S_OK;
}

void CRenderTarget::Render_Debug(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return;

    if (FAILED(pShader->Bind_SRV("g_Texture", m_pSRV)))
        return;

    if (FAILED(pShader->Begin(0)))
        return;

    if (FAILED(pVIBuffer->Bind_Resources()))
        return;

    if (FAILED(pVIBuffer->Render()))
        return;
}

#endif

CRenderTarget* CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

    if (FAILED(pInstance->Initialize(iSizeX, iSizeY, ePixelFormat, vClearColor)))
    {
        MSG_BOX("Failed to Created : CRenderTarget");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRenderTarget::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    Safe_Release(m_pSRV);
    Safe_Release(m_pRTV);
    Safe_Release(m_pTexture2D);
}
