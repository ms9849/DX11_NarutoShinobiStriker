#include "PipeLine.h"

#include "GameInstance.h"

CPipeLine::CPipeLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice  { pDevice }
    , m_pContext { pContext }
    , m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CPipeLine::Initialize()
{
    return S_OK;
}

void CPipeLine::Update()
{
}

const _float4x4& CPipeLine::Get_ViewMatrix()
{
    return m_ViewMatrix;
}

const _float4x4& CPipeLine::Get_CameraWorldMatrix()
{
    return m_CameraWorldMatrix;
}

const _float4x4& CPipeLine::Get_ProjMatrix()
{
    return m_ProjMatrix;
}

void CPipeLine::Set_CameraWorldMatrix(const _float4x4& CameraWorldMatrix)
{
    m_CameraWorldMatrix = CameraWorldMatrix;

    _matrix InvMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_CameraWorldMatrix));
    XMStoreFloat4x4(&m_ViewMatrix, InvMatrix);
}

void CPipeLine::Set_ProjMatrix(const _float4x4& ProjMatrix)
{
    m_ProjMatrix = ProjMatrix;
}

CPipeLine* CPipeLine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPipeLine* pInstance = new CPipeLine(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Create Failed : PipeLine");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPipeLine::Free()
{
    __super::Free();

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
    Safe_Release(m_pGameInstance);
}
