#include "SkillSlot.h"

#include "GameInstance.h"
#include "GameManager.h"

CSkillSlot::CSkillSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CUIObject{ pDevice, pContext, ENUM_CLASS(eObjectID) }
    , m_pGameManager { CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

CSkillSlot::CSkillSlot(const CSkillSlot& rhs)
    : CUIObject{ rhs }
    , m_pGameManager{ CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

HRESULT CSkillSlot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkillSlot::Initialize(void* pArg)
{
    m_iTextureIdx = 0;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CSkillSlot::Priority_Update(_float fTimeDelta)
{
}

void CSkillSlot::Update(_float fTimeDelta)
{
}

void CSkillSlot::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CSkillSlot::Render()
{
    __super::Render();

    return S_OK;
}

HRESULT CSkillSlot::Ready_Components()
{
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Prototype_Component_Texture_SkillSlotUI"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CSkillSlot::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

CSkillSlot* CSkillSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CSkillSlot* pInstance = new CSkillSlot(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CSkillSlot UI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSkillSlot::Clone(void* pArg)
{
    CSkillSlot* pInstance = new CSkillSlot(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CSkillSlot UI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSkillSlot::Free()
{
    __super::Free();
}
