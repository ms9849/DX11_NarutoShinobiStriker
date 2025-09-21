#include "SkillActionCamera.h"

#include "GameInstance.h"

CSkillActionCamera::CSkillActionCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CCamera { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CSkillActionCamera::CSkillActionCamera(const CSkillActionCamera& rhs)
    : CCamera{ rhs }
{
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
    else
    {
        m_pTransformCom->Set_State(STATE::POSITION, m_pPlayerTransform->Get_State(STATE::POSITION) + (m_pPlayerTransform->Get_State(STATE::LOOK) * -1.f) + m_pPlayerTransform->Get_State(STATE::RIGHT) + XMVectorSet(0.f, 1.5f, 0.f, 0.f));
        m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION) + m_pPlayerTransform->Get_State(STATE::LOOK) * 5.f);
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

void CSkillActionCamera::OnChange()
{
    if (SKILL::KAMUI == m_eSkillType)
    {
        m_pTransformCom->Set_State(STATE::POSITION, m_pPlayerTransform->Get_State(STATE::POSITION) + 3.f * m_pPlayerTransform->Get_State(STATE::LOOK) + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
        m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(STATE::POSITION) + XMVectorSet(0.f, 1.2f, 0.f, 0.f));
        m_fKamuiCamTimeAcc = 0.f;
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
}
