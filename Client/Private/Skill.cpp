#include "Skill.h"

#include "GameInstance.h"

CSkill::CSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CSkill::CSkill(const CSkill& rhs)
    : CGameObject { rhs }
{
}

HRESULT CSkill::Initialize_Prototype()
{
    return E_NOTIMPL;
}

HRESULT CSkill::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CSkill::Priority_Update(_float fTimeDelta)
{
}

void CSkill::Update(_float fTimeDelta)
{
}

void CSkill::Late_Update(_float fTimeDelta)
{
}

HRESULT CSkill::Render()
{
    return E_NOTIMPL;
}

void CSkill::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
