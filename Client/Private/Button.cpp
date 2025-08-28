#include "Button.h"

#include "GameInstance.h"

CButton::CButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CUIObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CButton::CButton(const CButton& rhs)
	: CUIObject { rhs }
{
}

HRESULT CButton::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CButton::Initialize(void* pArg)
{
	/* 부모로 받아온 녀석의 상대적인 위치로 세팅해줘야 할 것.. */

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/* 움직이는 UI는 없을 듯 함.. */
	m_rcButton.left = (LONG)m_fX - (LONG)(m_pTransformCom->Get_Scale().x / 2.f);
	m_rcButton.top = (LONG)m_fY - (LONG)(m_pTransformCom->Get_Scale().y / 2.f);
	m_rcButton.right = (LONG)m_fX + (LONG)(m_pTransformCom->Get_Scale().x / 2.f);
	m_rcButton.bottom = (LONG)m_fY + (LONG)(m_pTransformCom->Get_Scale().y / 2.f);

	return S_OK;
}

void CButton::Priority_Update(_float fTimeDelta)
{
}

void CButton::Update(_float fTimeDelta)
{
	if (m_bFadeOut)
		Play_Animation_FadeOut(fTimeDelta);

}

void CButton::Late_Update(_float fTimeDelta)
{
}

HRESULT CButton::Render()
{
	if (false == m_bVisible)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CButton::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (m_iShaderPassIdx == ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT) && m_bFadeOut)
		m_pShaderCom->Bind_Float("g_Alpha", 1 - (m_fFadeOutTimeAcc / m_fFadeOutMaxTimeAcc));

	//else if (m_iShaderPassIdx == ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT) && m_bFadeIn)
	//	m_pShaderCom->Bind_Float("g_Alpha", m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc);

	return S_OK;
}

void CButton::Trigger_FadeOut()
{
	m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT);
	m_bFadeOut = true;
}

_bool CButton::IsClicked()
{
	return IsHovered() && m_pGameInstance->Key_Down(VK_LBUTTON);
}

_bool CButton::IsHovered()
{
	return true;
	//return PtInRect(&m_rcButton, m_pGameInstance->Get_MousePos());
}

void CButton::Toggle_Focus()
{
	if (m_iTextureIdx == 0)
		m_iTextureIdx = 1;
	else
		m_iTextureIdx = 0;
}

void CButton::Play_Animation_FadeOut(_float fTimeDelta)
{
	m_fFadeOutTimeAcc += fTimeDelta;

	if (m_fFadeOutTimeAcc > m_fFadeOutMaxTimeAcc)
		m_fFadeOutTimeAcc = m_fFadeOutMaxTimeAcc;

	/* 애니메이션 종료 */
	if (m_fFadeOutTimeAcc >= m_fFadeOutMaxTimeAcc)
	{
		m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI);
		m_bFadeOut = false;
		m_fFadeOutTimeAcc = 0.f;
	}
}

void CButton::Free()
{
	__super::Free();
}
