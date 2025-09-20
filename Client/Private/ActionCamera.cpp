#include "ActionCamera.h"

CActionCamera::CActionCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CCamera { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CActionCamera::CActionCamera(const CActionCamera& rhs)
    : CCamera { rhs }
{
}

HRESULT CActionCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CActionCamera::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_vCameraPos = _float3{ 0.f, 4.f, -4.f };
    m_pPlayerTransform = static_cast<ACTION_CAMERA_DESC*>(pArg)->pPlayerTransform;
    m_pTransformCom->Set_State(STATE::POSITION, m_pPlayerTransform->Get_State(STATE::POSITION) + XMVectorSet(m_vCameraPos.x, m_vCameraPos.y, m_vCameraPos.z, 0.f));
    Safe_AddRef(m_pPlayerTransform);

    return S_OK;
}

void CActionCamera::Priority_Update(_float fTimeDelta)
{
    m_pTransformCom->Set_State(STATE::POSITION, m_pPlayerTransform->Get_State(STATE::POSITION) + (m_pPlayerTransform->Get_State(STATE::LOOK) * -1.f) + m_pPlayerTransform->Get_State(STATE::RIGHT) + XMVectorSet(0.f, 1.5f, 0.f, 0.f));

    m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION) + m_pPlayerTransform->Get_State(STATE::LOOK) * 5.f);


    __super::Bind_Matrices();
}

void CActionCamera::Update(_float fTimeDelta)
{
}

void CActionCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CActionCamera::Render()
{
    return S_OK;
}

CActionCamera* CActionCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CActionCamera* pInstance = new CActionCamera(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : Action Camera");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CActionCamera::Clone(void* pArg)
{
    CActionCamera* pInstance = new CActionCamera(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : Action Camera");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CActionCamera::Free()
{
    __super::Free();

    Safe_Release(m_pPlayerTransform);
}
