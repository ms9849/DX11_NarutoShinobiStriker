#include "NPCTalkCamera.h"

#include "GameInstance.h"

CNPCTalkCamera::CNPCTalkCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CCamera { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CNPCTalkCamera::CNPCTalkCamera(const CNPCTalkCamera& rhs)
    : CCamera { rhs }
{
}

HRESULT CNPCTalkCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNPCTalkCamera::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    NPC_TALK_CAMERA_DESC* pDesc = static_cast<NPC_TALK_CAMERA_DESC*>(pArg);

    m_pTargetTransform = pDesc->pTargetTransform;
    Safe_AddRef(m_pTargetTransform);

    return S_OK;
}

void CNPCTalkCamera::Priority_Update(_float fTimeDelta)
{
    m_pTransformCom->Set_State(STATE::POSITION
        , m_pTargetTransform->Get_State(STATE::POSITION) + m_pTargetTransform->Get_State(STATE::LOOK) * 1.4f + m_pTargetTransform->Get_State(STATE::RIGHT) * 0.3f + XMVectorSet(0.f, 1.35f, 0.f, 0.f));

    m_pTransformCom->LookAt(m_pTargetTransform->Get_State(STATE::POSITION) + m_pTargetTransform->Get_State(STATE::RIGHT) * 0.55f + XMVectorSet(0.f, 1.4f, 0.f, 0.f));

    __super::Bind_Matrices();
}

void CNPCTalkCamera::Update(_float fTimeDelta)
{
}

void CNPCTalkCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CNPCTalkCamera::Render()
{
    return S_OK;
}

CNPCTalkCamera* CNPCTalkCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CNPCTalkCamera* pInstance = new CNPCTalkCamera(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CNPCTalkCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CNPCTalkCamera::Clone(void* pArg)
{
    CNPCTalkCamera* pInstance = new CNPCTalkCamera(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CNPCTalkCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNPCTalkCamera::Free()
{
    __super::Free();

    Safe_Release(m_pTargetTransform);
}
