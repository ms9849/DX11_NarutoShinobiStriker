#include "ModelPanel.h"

#include "GameInstance.h"

#include "ModelSelectButtonUI.h"
#include "ModelDecideButtonUI.h"

CModelPanel::CModelPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CPanel { pDevice, pContext, eObjectID }
{
}

CModelPanel::CModelPanel(const CModelPanel& rhs)
    : CPanel{ rhs }
{
}

HRESULT CModelPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CModelPanel::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Buttons()))
        return E_FAIL;

    if (FAILED(Ready_DecideButton()))
        return E_FAIL;

    m_iFocusedNum = -1;
    m_eSelectType = SELECT_TYPE::PARTS;
    m_iMaxActivateNum = m_ButtonInfos[ENUM_CLASS(m_eSelectType)];
    Set_Visible_Buttons(m_iMaxActivateNum);

    return S_OK;
}

void CModelPanel::Priority_Update(_float fTimeDelta)
{
}

void CModelPanel::Update(_float fTimeDelta)
{
    if (m_IsFadeIn)
        Play_Animation_FadeIn(fTimeDelta);

    if (m_IsFadeOut)
        Play_Animation_FadeOut(fTimeDelta);

    Key_Input();
}

void CModelPanel::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CModelPanel::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CModelPanel::Play_Animation_FadeIn(_float fTimeDelta)
{
    m_fFadeInTimeAcc += fTimeDelta;

    if (m_fFadeInTimeAcc >= m_fFadeInMaxTimeAcc)
        m_fFadeInTimeAcc = m_fFadeInMaxTimeAcc;

    if (m_fFadeInTimeAcc <= m_fFadeInMaxTimeAcc)
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet((m_fX - g_iWinSizeX / 2.f - m_fAnimationDist * (1 - m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc)), -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));

    /* 애니메이션 종료 */
    if (m_fFadeInTimeAcc >= m_fFadeInMaxTimeAcc && 
        static_cast<CButton*>(m_Childs[m_iMaxActivateNum])->IsPlaying() == false)
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - g_iWinSizeX / 2.f, -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));
        m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI);
        m_IsFadeIn = false;
        m_fFadeInTimeAcc = 0.f;

        if (m_bChangeSelectType)
        {
            Change_SelectType();
            m_bChangeSelectType = false;
        }

        if (m_bBackToParts)
        {
            Back_ToParts();
            m_bBackToParts = false;
        }

        for (_int i = 0; i < m_Childs.size(); ++i)
            static_cast<CButton*>(m_Childs[i])->Toggle_ShowNormal();

    }
}

void CModelPanel::Play_Animation_FadeOut(_float fTimeDelta)
{
    m_fFadeOutTimeAcc += fTimeDelta;

    if (m_fFadeOutTimeAcc >= m_fFadeOutMaxTimeAcc)
        m_fFadeOutTimeAcc = m_fFadeOutMaxTimeAcc;

    if (m_fFadeOutTimeAcc <= m_fFadeOutMaxTimeAcc)
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet((m_fX - g_iWinSizeX / 2.f - m_fAnimationDist * (m_fFadeOutTimeAcc / m_fFadeOutMaxTimeAcc)), -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));

    /* 애니메이션 종료 */
    if (m_fFadeOutTimeAcc >= m_fFadeOutMaxTimeAcc && 
        static_cast<CButton*>(m_Childs[m_iMaxActivateNum - 1])->IsPlaying() == false)
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - g_iWinSizeX / 2.f, -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));
        m_IsFadeOut = false;
        m_fFadeOutTimeAcc = 0.f;
        //m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI);

        /* FadeOut 끝났으면 바로 FadeIn 켜버리기 */
        m_IsFadeIn = true;

        if (true == m_bChangeSelectType)
        {
            for (_int i = 0; i < m_ButtonInfos[m_iFocusedNum + 1]; ++i)
                static_cast<CButton*>(m_Childs[i])->Trigger_FadeIn((i + 1) * m_fButtonDelay);

            static_cast<CButton*>(m_Childs[m_iDecideButtonNum])->Set_Visible(false);

        }

        else if (true == m_bBackToParts)
        {
            for (_int i = 0; i < m_ButtonInfos[ENUM_CLASS(SELECT_TYPE::PARTS)]; ++i)
                static_cast<CButton*>(m_Childs[i])->Trigger_FadeIn((i + 1) * m_fButtonDelay);
            
            /* 결정 버튼도 애니메이션 재생해주기 */
            static_cast<CButton*>(m_Childs[m_iDecideButtonNum])->Trigger_FadeIn(0.f);
        }
    }
}

void CModelPanel::Change_FocusedButton(_int iNum)
{   
    if (m_eSelectType != SELECT_TYPE::PARTS)
    {
        if (m_iFocusedNum >= 0 && m_iFocusedNum <= m_iMaxActivateNum -1)
            static_cast<CButton*>(m_Childs[m_iFocusedNum])->Toggle_Focus();

        if (iNum > m_iMaxActivateNum - 1)
            iNum = 0;
        else if (iNum < 0)
            iNum = m_iMaxActivateNum - 1;

        if (iNum >= 0 && iNum <= m_iMaxActivateNum -1)
            static_cast<CButton*>(m_Childs[iNum])->Toggle_Focus();
    }
    else
    {
        if ((m_iFocusedNum >= 0 && m_iFocusedNum <= m_iMaxActivateNum - 1) ||
            (m_iFocusedNum == m_iDecideButtonNum))
            static_cast<CButton*>(m_Childs[m_iFocusedNum])->Toggle_Focus();

        // 아래 버튼 누른 상태라면.
        if (iNum > m_iMaxActivateNum - 1 && m_iFocusedNum == (iNum + 1))
            iNum = m_ButtonInfos[ENUM_CLASS(SELECT_TYPE::PARTS)] - 1;
        // 위 버튼 누른 상태라면 
        else if (iNum <= m_iDecideButtonNum && iNum > m_iMaxActivateNum - 1 && m_iFocusedNum == (iNum - 1) || iNum < 0)
            iNum = m_iDecideButtonNum;

        else if (iNum > m_iDecideButtonNum)
            iNum = 0;

        if ((iNum >= 0 && iNum <= m_iMaxActivateNum - 1) || iNum == m_iDecideButtonNum)
            static_cast<CButton*>(m_Childs[iNum])->Toggle_Focus();
    }

    m_iFocusedNum = iNum;
}


void CModelPanel::Change_SelectType()
{
     // SELECT TYPE 변경, 혹은 외형 변경
    if (m_iFocusedNum < 0)
        return;

    if (SELECT_TYPE::PARTS == m_eSelectType)
    {
        m_eSelectType = static_cast<SELECT_TYPE>(m_iFocusedNum + 1);
        m_iMaxActivateNum = m_ButtonInfos[ENUM_CLASS(m_eSelectType)];
    }

    /* 늘 포커스는 잃게해줘야 한다. */
    if (m_iFocusedNum != -1)
        static_cast<CButton*>(m_Childs[m_iFocusedNum])->Toggle_Focus();
    m_iFocusedNum = -1;
    Set_Visible_Buttons(m_iMaxActivateNum);
}

void CModelPanel::Back_ToParts()
{
    m_eSelectType = SELECT_TYPE::PARTS;
    m_iMaxActivateNum = m_ButtonInfos[ENUM_CLASS(SELECT_TYPE::PARTS)];


    /* 늘 포커스는 잃게해줘야 한다. */
    if(m_iFocusedNum != -1)
        static_cast<CButton*>(m_Childs[m_iFocusedNum])->Toggle_Focus();
    m_iFocusedNum = -1;
    Set_Visible_Buttons(m_iMaxActivateNum);
}

void CModelPanel::Key_Input()
{
    /* 애니메이션 재생중이면 키 입력 안받게 할 것. 변수 생긴다..*/
    if (true == m_IsFadeIn || true == m_IsFadeOut)
        return;

    if (m_pGameInstance->Key_Down(DIK_DOWN))
    {
        Change_FocusedButton(m_iFocusedNum + 1);
    }

    if (m_pGameInstance->Key_Down(DIK_UP))
    {
        Change_FocusedButton(m_iFocusedNum - 1);
    }

    if (m_pGameInstance->Key_Down(DIK_SPACE) && m_iFocusedNum != m_iDecideButtonNum)
    {
        if (m_iFocusedNum < 0 || m_IsFadeIn || m_IsFadeOut)
            return;

        if (SELECT_TYPE::PARTS == m_eSelectType)
        {
            m_IsFadeOut = true;
            m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT);

            for (_int i = 0; i < m_iMaxActivateNum; ++i)
                static_cast<CButton*>(m_Childs[i])->Trigger_FadeOut((i + 1) * m_fButtonDelay);

            for (_int i = 0; i < m_Childs.size(); ++i)
                static_cast<CButton*>(m_Childs[i])->Toggle_ShowNormal();

            if (SELECT_TYPE::PARTS == m_eSelectType)
                static_cast<CButton*>(m_Childs[m_iDecideButtonNum])->Trigger_FadeOut(0.f);

            m_bChangeSelectType = true;
        }
        else
        {
            /* 여기서 외형 바꿔주면 될 것 */
        }
    }

    if (m_pGameInstance->Key_Down(DIK_ESCAPE))
    {
        if (m_eSelectType == SELECT_TYPE::PARTS || m_IsFadeIn || m_IsFadeOut)
            return;

        m_IsFadeOut = true;
        m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT);

        for (_int i = 0; i < m_iMaxActivateNum; ++i)
            static_cast<CButton*>(m_Childs[i])->Trigger_FadeOut((i + 1) * m_fButtonDelay);

        for (_int i = 0; i < m_Childs.size(); ++i)
            static_cast<CButton*>(m_Childs[i])->Toggle_ShowNormal();

        m_bBackToParts = true;
    }
}

void CModelPanel::Set_Visible_Buttons(_uint iMaxIdx)
{
    for (_uint i = 0; i < iMaxIdx; ++i)
        static_cast<CButton*>(m_Childs[i])->Set_Visible(true);

    for (_uint i = iMaxIdx; i < m_Childs.size(); ++i)
        static_cast<CButton*>(m_Childs[i])->Set_Visible(false);

    /* 결정 버튼 */
    if(m_eSelectType == SELECT_TYPE::PARTS)
        static_cast<CButton*>(m_Childs[m_iDecideButtonNum])->Set_Visible(true);
}

HRESULT CModelPanel::Ready_Components()
{
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_Component_Texture_ModelPanel"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CModelPanel::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    if (m_iShaderPassIdx == ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT) && m_IsFadeIn)
    {
        _float fAlphaValue = m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc;
        m_pShaderCom->Bind_RawValue("g_Alpha", &fAlphaValue, sizeof(_float));
    }

    if (m_iShaderPassIdx == ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT) && m_IsFadeOut)
    {
        _float fAlphaValue = 1 - (m_fFadeOutTimeAcc / m_fFadeOutMaxTimeAcc);
        m_pShaderCom->Bind_RawValue("g_Alpha", &fAlphaValue, sizeof(_float));
    }

    return S_OK;
}

HRESULT CModelPanel::Ready_Buttons()
{
    UIOBJECT_DESC Desc;
    CModelSelectButtonUI* pModelSelectButton;

    /* 테스트용 모델 외형 결정 버튼 */
    for (_uint i = 0; i < 8; ++i)
    {
        Desc = CUIObject::CreateDesc(g_iWinSizeX / 2.f + 300.f, g_iWinSizeY / 2.f - 100.f + 47.f * i, m_fZ - 0.05f, 380.f, 40.f, 0, 0.f);

        pModelSelectButton = static_cast<CModelSelectButtonUI*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_GameObject_ModelSelectButtonUI"), &Desc));
        m_pGameInstance->Add_Clone_ToLayer(pModelSelectButton, ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Layer_UI"));
    
        m_Childs.push_back(pModelSelectButton);
        Safe_AddRef(pModelSelectButton);
    }

    return S_OK;
}

HRESULT CModelPanel::Ready_DecideButton()
{
    UIOBJECT_DESC Desc;

    /* 테스트용 모델 외형 결정 버튼 */
    Desc = CUIObject::CreateDesc(g_iWinSizeX / 2.f + 300.f, g_iWinSizeY / 2.f + 300.f, m_fZ - 0.05f, 250.f, 50.f, 0, 0.f);

    CModelDecideButtonUI* pModelDecideButton = static_cast<CModelDecideButtonUI*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_GameObject_ModelDecideButtonUI"), &Desc));
    m_pGameInstance->Add_Clone_ToLayer(pModelDecideButton, ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Layer_UI"));

    m_Childs.push_back(pModelDecideButton);
    Safe_AddRef(pModelDecideButton);

    /* 마지막 인덱스는 외형 결정 버튼이다. */
    m_iDecideButtonNum = m_Childs.size() - 1;

    return S_OK;
}

CModelPanel* CModelPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CModelPanel* pInstance = new CModelPanel(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CModelPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CModelPanel::Clone(void* pArg)
{
    CModelPanel* pInstance = new CModelPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CModelPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CModelPanel::Free()
{
    __super::Free();

    Safe_Release(m_pDecideButton);
}
