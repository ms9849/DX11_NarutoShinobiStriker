#include "ModelDecideButtonUI.h"

#include "GameInstance.h"

CModelDecideButtonUI::CModelDecideButtonUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CButton { pDevice, pContext, eObjectID }
{
}

CModelDecideButtonUI::CModelDecideButtonUI(const CModelDecideButtonUI& rhs)
	: CButton { rhs }
{
}

HRESULT CModelDecideButtonUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CModelDecideButtonUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CModelDecideButtonUI::Priority_Update(_float fTimeDelta)
{
}

void CModelDecideButtonUI::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CModelDecideButtonUI::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CModelDecideButtonUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModelDecideButtonUI::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_Component_Texture_ModelDecideButtonUI"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CModelDecideButtonUI::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

CModelDecideButtonUI* CModelDecideButtonUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CModelDecideButtonUI* pInstance = new CModelDecideButtonUI(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : Model Decide Button");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CModelDecideButtonUI::Clone(void* pArg)
{
	CModelDecideButtonUI* pInstance = new CModelDecideButtonUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : Model Decide Button");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModelDecideButtonUI::Free()
{
	__super::Free();
}
