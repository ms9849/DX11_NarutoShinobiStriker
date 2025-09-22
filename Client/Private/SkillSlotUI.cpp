#include "SkillSlotUI.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Skill_Table.h"
#include "Player.h"

#include "Effect_CoolDown.h"

CSkillSlotUI::CSkillSlotUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CUIObject{ pDevice, pContext, ENUM_CLASS(eObjectID) }
    , m_pGameManager { CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

CSkillSlotUI::CSkillSlotUI(const CSkillSlotUI& rhs)
    : CUIObject{ rhs }
    , m_pGameManager{ CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

void CSkillSlotUI::Change_Skill(SKILL eSkill)
{
    /* 여기서 나중에 이펙트 뿌려줄 수 있게 세팅해줘야지. */
    m_iTextureIdx_Skill = ENUM_CLASS(eSkill);
    m_eSkill = eSkill;

    SKILL_INFO* pInfo = m_pGameManager->Get_PlayerPtr()->Get_Skill_Info(m_eSkill);

    m_fPreSkillTimeAcc = 0.f;
    m_fSkillTimeAcc = pInfo->fTimeAcc;
    m_fMaxSkillCoolDown = pInfo->fMaxCoolDown;
}

void CSkillSlotUI::Set_Visible(_bool bFlag)
{
    m_IsVisible = bFlag;
}

HRESULT CSkillSlotUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkillSlotUI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_iSkillNum = static_cast<SKILLSLOT_DESC*>(pArg)->iSkillNum;

    if (m_iSkillNum == ENUM_CLASS(SKILLNUM::SPECIAL))
    {
        m_iTextureIdx = 1;
        m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_SPECIALSKILL);
    }
    else
    {
        m_iTextureIdx = 0;
        m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_SKILL);
    }


    return S_OK;
}

void CSkillSlotUI::Priority_Update(_float fTimeDelta)
{
}

void CSkillSlotUI::Update(_float fTimeDelta)
{
    SKILL_INFO* pInfo = m_pGameManager->Get_PlayerPtr()->Get_Skill_Info(m_eSkill);

    m_fPreSkillTimeAcc = m_fSkillTimeAcc;
    m_fSkillTimeAcc = pInfo->fTimeAcc;
    m_fMaxSkillCoolDown = pInfo->fMaxCoolDown;

    if (ENUM_CLASS(SKILLNUM::SPECIAL) != m_iSkillNum && m_fSkillTimeAcc >= m_fMaxSkillCoolDown && m_fPreSkillTimeAcc <= m_fMaxSkillCoolDown)
    {
        CEffect_CoolDown::EFFECT_COOLDOWN_DESC Desc{};
        _vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);

        Desc.fX = XMVectorGetX(vPosition);
        Desc.fY = XMVectorGetY(vPosition);

        m_pGameInstance->Add_PoolingObject_ToLayer(TEXT("Effect_SkillCoolDown"), ENUM_CLASS(LEVEL::GAMEPLAY),
            &Desc, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Effect"));
    }
}

void CSkillSlotUI::Late_Update(_float fTimeDelta)
{
    if (false == m_IsVisible)
        return;

    m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CSkillSlotUI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSkillSlotUI::Ready_Components()
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

    if (FAILED(__super::Add_Component(ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Prototype_Component_Texture_Skill_Icon"),
        TEXT("Com_Texture_Skill"), reinterpret_cast<CComponent**>(&m_pTextureCom_Skill))))
        return E_FAIL;

    return S_OK;
}

HRESULT CSkillSlotUI::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pTextureCom_Skill->Bind_ShaderResource(m_pShaderCom, "g_Texture_Skill", m_iTextureIdx_Skill)))
        return E_FAIL;

    if (m_iSkillNum != ENUM_CLASS(SKILLNUM::SPECIAL))
    {

        if (FAILED(m_pShaderCom->Bind_RawValue("g_SkillCoolDown", &m_fSkillTimeAcc, sizeof(_float))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_MaxSkillCoolDown", &m_fMaxSkillCoolDown, sizeof(_float))))
            return E_FAIL;
    }

    return S_OK;
}

CSkillSlotUI* CSkillSlotUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CSkillSlotUI* pInstance = new CSkillSlotUI(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CSkillSlot UI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSkillSlotUI::Clone(void* pArg)
{
    CSkillSlotUI* pInstance = new CSkillSlotUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CSkillSlot UI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSkillSlotUI::Free()
{
    __super::Free();

    Safe_Release(m_pGameManager);
    Safe_Release(m_pTextureCom_Skill);
}
