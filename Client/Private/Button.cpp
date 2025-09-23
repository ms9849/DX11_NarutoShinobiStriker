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
	if (m_IsFadeOut && (m_fReserveTime < m_fTimeAcc))
		Play_Animation_FadeOut(fTimeDelta);

	if (m_IsFadeIn && (m_fReserveTime < m_fTimeAcc))
		Play_Animation_FadeIn(fTimeDelta);

	m_fTimeAcc += fTimeDelta;
}

void CButton::Late_Update(_float fTimeDelta)
{
}

HRESULT CButton::Render()
{
	if (false == m_IsVisible)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CButton::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (true == m_bShowNormal)
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIdx)))
			return E_FAIL;
	}

	if (m_iShaderPassIdx == ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT) && m_IsFadeOut)
	{
		_float fAlphaValue = 1 - (m_fFadeOutTimeAcc / m_fFadeOutMaxTimeAcc);
		m_pShaderCom->Bind_RawValue("g_Alpha", &fAlphaValue, sizeof(_float));
	}

	else if (m_iShaderPassIdx == ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT) && m_IsFadeIn)
	{
		_float fAlphaValue = m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc;
		m_pShaderCom->Bind_RawValue("g_Alpha", &fAlphaValue, sizeof(_float));
	}

	return S_OK;
}

_bool CButton::IsClicked()
{
	return IsHovered() && m_pGameInstance->Key_Down(VK_LBUTTON);
}

_bool CButton::IsHovered()
{
	return true;
}

void CButton::Toggle_Focus()
{
	if (m_iTextureIdx == 0)
		m_iTextureIdx = 1;
	else
		m_iTextureIdx = 0;
}

void CButton::Trigger_FadeIn(_float fReserveTime)
{
	m_fReserveTime = fReserveTime;
	m_fTimeAcc = 0.f;

	m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT);
	m_IsVisible = true;
	m_IsFadeIn = true;
}

void CButton::Trigger_FadeOut(_float fReserveTime)
{
	m_fReserveTime = fReserveTime;
	m_fTimeAcc = 0.f;

	m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT);
	m_IsFadeOut = true;
}

void CButton::Play_Animation_FadeOut(_float fTimeDelta)
{
	m_fFadeOutTimeAcc += fTimeDelta;

	if (m_fFadeOutTimeAcc >= m_fFadeOutMaxTimeAcc)
		m_fFadeOutTimeAcc = m_fFadeOutMaxTimeAcc;

	else if (m_fFadeOutTimeAcc < m_fFadeOutMaxTimeAcc)
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet((m_fX - g_iWinSizeX / 2.f - m_fAnimationDist * (m_fFadeOutTimeAcc / m_fFadeOutMaxTimeAcc)), -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));

	/* 애니메이션 종료 */
	if (m_fFadeOutTimeAcc >= m_fFadeOutMaxTimeAcc)
	{
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - g_iWinSizeX / 2.f, -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));
		m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI);
		m_IsFadeOut = false;
		m_IsVisible = false;
		m_fFadeOutTimeAcc = 0.f;
		m_fTimeAcc = 0.f;
	}
}

void CButton::Play_Animation_FadeIn(_float fTimeDelta)
{
	m_fFadeInTimeAcc += fTimeDelta;

	if (m_fFadeInTimeAcc >= m_fFadeInMaxTimeAcc)
		m_fFadeInTimeAcc = m_fFadeInMaxTimeAcc;

	else if (m_fFadeInTimeAcc < m_fFadeInMaxTimeAcc)
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet((m_fX - g_iWinSizeX / 2.f - m_fAnimationDist * (1 - m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc)), -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));

	/* 애니메이션 종료 */
	if (m_fFadeInTimeAcc >= m_fFadeInMaxTimeAcc)
	{
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - g_iWinSizeX / 2.f, -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));
		m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI);
		m_IsFadeIn = false;
		m_fFadeInTimeAcc = 0.f;
		m_fTimeAcc = 0.f;
	}
}

void CButton::Free()
{
	__super::Free();
}
