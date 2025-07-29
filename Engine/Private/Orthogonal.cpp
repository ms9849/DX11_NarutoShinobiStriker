#include "Orthogonal.h"

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

void COrthogonal::Orthogonal_Begin()
{
    //m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, FALSE);

    //m_pGraphic_Device->GetTransform(D3DTS_VIEW, &m_OldViewMatrix);
    //m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &m_OldProjMatrix);

    //m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
    //m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);
}

void COrthogonal::Orthogonal_End()
{
    //m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);

    //m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_OldViewMatrix);
    //m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_OldProjMatrix);
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
