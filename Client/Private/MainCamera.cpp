#include "MainCamera.h"

#include "GameManager.h"

CMainCamera::CMainCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CCamera { pDevice, pContext, ENUM_CLASS(eObjectID) }
    , m_pGameManager { CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

CMainCamera::CMainCamera(const CMainCamera& rhs)
    : CCamera{ rhs }
    , m_pGameManager { CGameManager::GetInstance() }
{
}

HRESULT CMainCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMainCamera::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pPlayerTransform = static_cast<MAIN_CAMERA_DESC*>(pArg)->pPlayerTransform;
    m_pTransformCom->Set_State(STATE::POSITION, m_pPlayerTransform->Get_State(STATE::POSITION) + XMVectorSet(0.f, 4.f, -4.f, 0.f));
    Safe_AddRef(m_pPlayerTransform);

    return S_OK;
}

void CMainCamera::Priority_Update(_float fTimeDelta)
{
    m_pTransformCom->Set_State(STATE::POSITION, m_pPlayerTransform->Get_State(STATE::POSITION) + XMVectorSet(0.f, 4.f, -4.f, 0.f));
    m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION));

    __super::Bind_Matrices();
}

void CMainCamera::Update(_float fTimeDelta)
{
}

void CMainCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CMainCamera::Render()
{
    return S_OK;
}

CMainCamera* CMainCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CMainCamera* pInstance = new CMainCamera(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : MainCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMainCamera::Clone(void* pArg)
{
    CMainCamera* pInstance = new CMainCamera(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clones Failed : MainCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMainCamera::Free()
{
    __super::Free();

    Safe_Release(m_pGameManager);
    Safe_Release(m_pPlayerTransform);
}
