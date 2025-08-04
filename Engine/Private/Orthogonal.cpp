#include "Orthogonal.h"
#include "Shader.h"

COrthogonal::COrthogonal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent { pDevice, pContext }
{
}

COrthogonal::COrthogonal(const COrthogonal& Prototype)
    : CComponent { Prototype }
{
}

HRESULT COrthogonal::Initialize_Prototype()
{
    return S_OK;
}

HRESULT COrthogonal::Initialize(void* pArg)
{
    D3D11_VIEWPORT       ViewportDesc{};
    _uint                iNumViewports = { 1 };

    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    XMStoreFloat4x4(&m_matView, XMMatrixIdentity());
    XMStoreFloat4x4(&m_matProj, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

    return S_OK;
}

HRESULT COrthogonal::Bind_ViewMatrix(CShader* pShader, const _char* pConstantName)
{
    return pShader->Bind_Matrix(pConstantName, &m_matView);
}

HRESULT COrthogonal::Bind_ProjMatrix(CShader* pShader, const _char* pConstantName)
{
    return pShader->Bind_Matrix(pConstantName, &m_matProj);
}

COrthogonal* COrthogonal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    COrthogonal* pInstance = new COrthogonal(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : Orthogonal Component!");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* COrthogonal::Clone(void* pArg)
{
    COrthogonal* pInstance = new COrthogonal(*this);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Clone Failed : Orthogonal Component!");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void COrthogonal::Free()
{
    __super::Free();
}
