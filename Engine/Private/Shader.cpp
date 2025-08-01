#include "Shader.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) 
    : CComponent {pDevice, pContext }
{
}

CShader::CShader(const CShader& rhs) 
    : CComponent { rhs }
    , m_pEffect { rhs.m_pEffect }
{
    Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
    return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
    return S_OK;
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
    CShader* pInstance = new CShader(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElements, iNumElements)))
    {
        MSG_BOX("Create Failed! : CShader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
    CShader* pInstance = new CShader(*this);

    if(FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed! : CShader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CShader::Free()
{
    __super::Free();

    Safe_Release(m_pEffect);
}
