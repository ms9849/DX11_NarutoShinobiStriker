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
}

void CButton::Late_Update(_float fTimeDelta)
{
}

HRESULT CButton::Render()
{
	return S_OK;
}

_bool CButton::IsClicked()
{
	return IsHovered() && m_pGameInstance->Key_Down(VK_LBUTTON);
}

_bool CButton::IsHovered()
{
	return PtInRect(&m_rcButton, m_pGameInstance->Get_MousePos());
}

void CButton::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
