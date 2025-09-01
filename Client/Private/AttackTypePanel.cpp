#include "AttackTypePanel.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"

CAttackTypePanel::CAttackTypePanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CPanel { pDevice, pContext, eObjectID }
{
}

CAttackTypePanel::CAttackTypePanel(const CAttackTypePanel& rhs)
    : CPanel { rhs }
{
}

void CAttackTypePanel::Change_AttackType(ATTACK_TYPE eAttackType)
{
    /* 애니메이션도 여기서 재생 시켜줌 */
    m_iTextureIdx = ENUM_CLASS(eAttackType);

    switch (eAttackType)
    {
    case ATTACK_TYPE::MELEE:
        m_strFontText = TEXT("근접형");
        break;

    case ATTACK_TYPE::NINJUTSU:
        m_strFontText = TEXT("인술형");
        break;

    case ATTACK_TYPE::DEFENSIVE:
        m_strFontText = TEXT("방어형");
        break;

    default:
        break;
    }

    m_fTimeAcc = 0.f;
    m_bTriggered = true;
    m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT);
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet((m_fX - g_iWinSizeX / 2.f - m_fAnimationDist * (1 - m_fTimeAcc / m_fMaxTimeAcc)), -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));
}

HRESULT CAttackTypePanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAttackTypePanel::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Texts()))
        return E_FAIL;

    m_pGameManager->Get_PlayerPtr()->Set_AttackTypePanel(this);

    return S_OK;
}

void CAttackTypePanel::Priority_Update(_float fTimeDelta)
{
}

void CAttackTypePanel::Update(_float fTimeDelta)
{
    if(m_bTriggered)
        Play_Animation_FadeIn(fTimeDelta);
}

void CAttackTypePanel::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::UI, this);

    /* 폰트 추가 */
    _float4 vPosition = m_pTransformCom->Get_State_Float4(STATE::POSITION);
    _float fAlpha = 1.f;

    if (m_bTriggered)
        fAlpha = m_fTimeAcc / m_fMaxTimeAcc;

    m_pFontCom->Bind_Resources(m_strFontText.c_str(), _float2{vPosition.x, vPosition.y},
        0.5f, XMVectorSet(1.f, 1.f, 1.f, fAlpha));

    m_pGameInstance->Add_Font(m_pFontCom);
}

HRESULT CAttackTypePanel::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CAttackTypePanel::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    if (m_iShaderPassIdx == ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT))
        m_pShaderCom->Bind_Float("g_Alpha", m_fTimeAcc / m_fMaxTimeAcc);

    return S_OK;
}

HRESULT CAttackTypePanel::Ready_Components()
{
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Prototype_Component_Texture_AttackTypePanel"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Font"),
        TEXT("Com_Font"), reinterpret_cast<CComponent**>(&m_pFontCom))))
        return E_FAIL;


    return S_OK;
}

HRESULT CAttackTypePanel::Ready_Texts()
{
    return S_OK;
}

void CAttackTypePanel::Play_Animation_FadeIn(_float fTimeDelta)
{
    m_fTimeAcc += fTimeDelta;
    
    if (m_fTimeAcc < m_fMaxTimeAcc)
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet((m_fX - g_iWinSizeX / 2.f - m_fAnimationDist * (1 - m_fTimeAcc / m_fMaxTimeAcc)), -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));
    }
    /* 애니메이션 종료 */
    else if(m_fTimeAcc >= m_fMaxTimeAcc)
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - g_iWinSizeX / 2.f, -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));
        m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI);
        m_bTriggered = false;
        m_fTimeAcc = 0.f;
    }
}

CAttackTypePanel* CAttackTypePanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CAttackTypePanel* pInstance = new CAttackTypePanel(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : AttackTypePanelUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAttackTypePanel::Clone(void* pArg)
{
    CAttackTypePanel* pInstance = new CAttackTypePanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : AttackTypePanelUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAttackTypePanel::Free()
{
    __super::Free();

    Safe_Release(m_pFontCom);
}
