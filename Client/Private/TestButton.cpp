#include "TestButton.h"

#include "GameInstance.h"

CTestButton::CTestButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CButton { pDevice, pContext, eObjectID }
{
}

CTestButton::CTestButton(const CTestButton& rhs)
	: CButton { rhs }
{
}

HRESULT CTestButton::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTestButton::Initialize(void* pArg)
{
	/* 부모로 받아온 녀석의 상대적인 위치로 세팅해줘야 할 것.. */

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CTestButton::Priority_Update(_float fTimeDelta)
{
}

void CTestButton::Update(_float fTimeDelta)
{
	if (true == __super::IsClicked())
		int a = 10;
	
	if (true == __super::IsHovered())
		int a = 10;
}

void CTestButton::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CTestButton::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTestButton::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_UI_Button"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTestButton::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

CTestButton* CTestButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CTestButton* pInstance = new CTestButton(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : TestButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTestButton::Clone(void* pArg)
{
	CTestButton* pInstance = new CTestButton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : TestButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestButton::Free()
{
	__super::Free();
}
