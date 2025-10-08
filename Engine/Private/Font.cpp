#include "Font.h"

#include "Shader.h"

CFont::CFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent { pDevice, pContext }
{
}

/* Sprite, Batch는 DXTK에서 제공되는 타입이라 AddRef, Release가 불가능함 */
/* 따라서 얕은복사로 하되, 레퍼런스 카운트로 관리하지 않고 */
/* Release 할때, 원형인지 체크해서 메모리 지워주게 해줌 */
CFont::CFont(const CFont& rhs)
    : CComponent { rhs }
    , m_pFont { rhs.m_pFont }
    , m_pBatch { rhs.m_pBatch }
    , m_fWinSizeX { rhs.m_fWinSizeX }
    , m_fWinSizeY { rhs.m_fWinSizeY }
{
}

HRESULT CFont::Initialize_Prototype(const _tchar* pFontFilePath)
{
    //여기서 폰트 파일 관련된 것들 작성

    m_pBatch = new SpriteBatch(m_pContext);
    if (nullptr == m_pBatch)
        return E_FAIL;

    m_pFont = new SpriteFont(m_pDevice, pFontFilePath);
    if (nullptr == m_pFont)
        return E_FAIL;

    CommonStates* BatchState = new CommonStates(m_pDevice);
    if (nullptr == BatchState)
        return E_FAIL;

    Safe_Delete(BatchState);

    D3D11_VIEWPORT ViewPortDesc{};
    //앞으로는 뷰포트가 여러개가 될거고, 갯수를 입력받는게 당연해질 것이라고 하심.
    _uint          iNumViewPorts = { 1 };

    m_pContext->RSGetViewports(&iNumViewPorts, &ViewPortDesc);
    m_fWinSizeX = ViewPortDesc.Width;
    m_fWinSizeY = ViewPortDesc.Height;

    return S_OK;
}

HRESULT CFont::Initialize(void* pArg)
{

    return S_OK;
}

HRESULT CFont::Bind_Resources(const _tchar* pText, const _float2& vPosition, _bool IsAlign, _float fScale, _fvector vColor, _float fRotation, const _float2& vOrigin)
{
    m_Text = _wstring(pText);
    
    m_fScale = fScale;

    _float2 vTextSize;
    _vector vDest = m_pFont->MeasureString(pText);
    vDest *= m_fScale;

    XMStoreFloat2(&vTextSize, vDest);

    if(true == IsAlign)
        m_vPosition = _float2(m_fWinSizeX / 2.f + vPosition.x - vTextSize.x / 2.f, m_fWinSizeY / 2.f - vPosition.y - vTextSize.y / 2.f );

    else
        m_vPosition = _float2(m_fWinSizeX / 2.f + vPosition.x, m_fWinSizeY / 2.f - vPosition.y);

    m_vColor = vColor;
    m_fRotation = fRotation;
    m_vOrigin = vOrigin;

    return S_OK;
}

HRESULT CFont::DrawFont()
{
    m_pContext->GSSetShader(nullptr, nullptr, 0);

    m_pBatch->Begin();

    m_pFont->DrawString(m_pBatch, m_Text.c_str(), m_vPosition, m_vColor, m_fRotation, m_vOrigin, m_fScale);

    m_pBatch->End();

    return S_OK;
}

CFont* CFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath)
{
    CFont* pInstance = new CFont(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pFontFilePath)))
    {
        MSG_BOX("Create Failed! : CFont");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CFont::Clone(void* pArg)
{
    CFont* pInstance = new CFont(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed! : CFont");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFont::Free()
{
    __super::Free();
    
    if (m_isCloned == false)
    {
        Safe_Delete(m_pFont);
        Safe_Delete(m_pBatch);
    }
}
