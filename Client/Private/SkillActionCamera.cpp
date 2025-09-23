#include "SkillActionCamera.h"

#include "GameInstance.h"
#include "GameManager.h"

CSkillActionCamera::CSkillActionCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CCamera { pDevice, pContext, ENUM_CLASS(eObjectID) }
    , m_pGameManager { CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

CSkillActionCamera::CSkillActionCamera(const CSkillActionCamera& rhs)
    : CCamera{ rhs }
    , m_pGameManager{ CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

HRESULT CSkillActionCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkillActionCamera::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    SKILL_ACTION_CAMERA_DESC* pDesc = static_cast<SKILL_ACTION_CAMERA_DESC*>(pArg);

    m_eSkillType = pDesc->eSkillType;
    m_pPlayerTransform = pDesc->pPlayerTransform;
    Safe_AddRef(m_pPlayerTransform);

    return S_OK;
}

void CSkillActionCamera::Priority_Update(_float fTimeDelta)
{
    if (SKILL::KAMUI == m_eSkillType)
    {
        if(m_fKamuiCamTimeAcc < m_fMaxKamuiCamTimeAcc)
            m_pTransformCom->Go_Straight(fTimeDelta * m_fKamuiCamSpeedRatio);

        m_fKamuiCamTimeAcc += fTimeDelta;
    }

    else if (SKILL::FIREBALL == m_eSkillType && true == m_IsZoom)
    {
        m_pTransformCom->Go_Straight(m_fFireBallCamSpeedRatio * fTimeDelta, nullptr);

        m_fZoomTimeAcc += fTimeDelta;

        if (m_fZoomTimeAcc >= m_fMaxZoomTimeAcc)
        {
            m_fZoomTimeAcc = 0.f;
            m_IsZoom = false;
            m_pGameManager->Change_Camera(static_cast<LEVEL>(m_pGameInstance->Get_LevelID()), TEXT("Main_Camera"), m_pGameInstance->Get_PipeLine_InverseFloat4x4(D3DTS::VIEW));
        }
    }
    else if (SKILL::CHIDORI == m_eSkillType)
    {
        m_fChidoriCamTimeAcc += fTimeDelta;
        m_pTransformCom->Set_State(STATE::POSITION, m_pPlayerTransform->Get_State(STATE::POSITION) + 2.f * m_pPlayerTransform->Get_State(STATE::LOOK) + XMVectorSet(0.f, 1.0f, 0.f, 0.f));
        m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION) + XMVectorSet(0.f, 1.0f, 0.f, 0.f));
    
        if (m_fMaxChidoriCamTimeAcc <= m_fChidoriCamTimeAcc)
        {
            m_fChidoriCamTimeAcc = 0.f;
            m_pGameManager->Change_Camera(static_cast<LEVEL>(m_pGameInstance->Get_LevelID()), TEXT("Main_Camera"), m_pGameInstance->Get_PipeLine_InverseFloat4x4(D3DTS::VIEW));
        }
    }
    __super::Bind_Matrices();
}

void CSkillActionCamera::Update(_float fTimeDelta)
{
}

void CSkillActionCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CSkillActionCamera::Render()
{
	return S_OK;
}

void CSkillActionCamera::OnChange(const _float4x4* pWorldMatrix)
{
    if (SKILL::KAMUI == m_eSkillType)
    {
        m_pTransformCom->Set_State(STATE::POSITION, m_pPlayerTransform->Get_State(STATE::POSITION) + 3.f * m_pPlayerTransform->Get_State(STATE::LOOK) + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
        m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION) + XMVectorSet(0.f, 1.2f, 0.f, 0.f));
        m_fKamuiCamTimeAcc = 0.f;
    }

    else if (SKILL::FIREBALL == m_eSkillType)
    {
        if(nullptr != pWorldMatrix)
            m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(pWorldMatrix));

        m_IsZoom = true;
    }

    else if (SKILL::BIG_SHARK == m_eSkillType)
    {
        m_pTransformCom->Set_State(STATE::POSITION, m_pPlayerTransform->Get_State(STATE::POSITION) + m_pPlayerTransform->Get_State(STATE::LOOK) + XMVectorSet(0.f, 1.2f, 0.f, 0.f));
        m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION) + XMVectorSet(0.f, 1.2f, 0.f, 0.f));
    }

    else if (SKILL::CHIDORI == m_eSkillType)
    {
        m_pTransformCom->Set_State(STATE::POSITION, m_pPlayerTransform->Get_State(STATE::POSITION) + 2.f * m_pPlayerTransform->Get_State(STATE::LOOK) + XMVectorSet(0.f, 1.0f, 0.f, 0.f));
        m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION) + XMVectorSet(0.f, 1.0f, 0.f, 0.f));
    }
}

CSkillActionCamera* CSkillActionCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CSkillActionCamera* pInstance = new CSkillActionCamera(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CSkillActionCamera");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSkillActionCamera::Clone(void* pArg)
{
    CSkillActionCamera* pInstance = new CSkillActionCamera(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CSkillActionCamera");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSkillActionCamera::Free()
{
    __super::Free();

    Safe_Release(m_pPlayerTransform);
    Safe_Release(m_pGameManager);
}
