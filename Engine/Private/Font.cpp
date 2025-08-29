#include "Font.h"

#include "Shader.h"

CFont::CFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent { pDevice, pContext }
{
}

/* Sprite, Batch는 DXTK에서 제공되는 타입이라 AddRef, Release가 불가능함 */
/* 따라서 얕은복사로 하되, 레퍼런스 카운트로 관리하지 않고 Release 할때, */
/* 원형인지 체크해서 메모리 지워주게 해줌 */
CFont::CFont(const CFont& rhs)
    : CComponent { rhs }
    , m_pFont { rhs.m_pFont }
    , m_pBatch { rhs.m_pBatch }
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

    return S_OK;
}

HRESULT CFont::Initialize(void* pArg)
{

    return S_OK;
}

HRESULT CFont::Bind_Resources(const _tchar* pText, const _float2& vPosition, _float fAlpha, _float fScale, _fvector vColor, _float fRotation, const _float2& vOrigin)
{
    m_pText = pText;
    m_vPosition = vPosition;
    m_fScale = fScale;
    m_vColor = vColor;
    m_fRotation = fRotation;
    m_vOrigin = vOrigin;
    m_fAlpha = fAlpha;

    return S_OK;
}

HRESULT CFont::DrawFont()
{
    /* 이펙트를 어떻게 넘겨주지..? */
    /* 폰트에 따라 서로 다른 이펙트를 받긴 해야 하니까.. */
    /* 그렇다고 셰이더에 종속시키고 싶진 않고.. */
    // -> 알파값만 받아올 것
    m_pBatch->Begin();

    m_pFont->DrawString(m_pBatch, m_pText, m_vPosition, m_vColor, m_fRotation, m_vOrigin, m_fScale);

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
        MSG_BOX("Create Failed! : CFont");
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
