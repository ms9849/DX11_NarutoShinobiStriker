#include "ProgressBarUI.h"

#include "GameInstance.h"

CProgressBarUI::CProgressBarUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CUIObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CProgressBarUI::CProgressBarUI(const CProgressBarUI& rhs)
	: CUIObject { rhs }
{
}

HRESULT CProgressBarUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProgressBarUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;;

	m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_PROGRESSBAR);
	m_fMaxSize = static_cast<UIOBJECT_DESC*>(pArg)->fSizeX;

	XMStoreFloat3(&m_vOriginPos, m_pTransformCom->Get_State(STATE::POSITION));

	return S_OK;
}

void CProgressBarUI::Priority_Update(_float fTimeDelta)
{
}

void CProgressBarUI::Update(_float fTimeDelta)
{
}

void CProgressBarUI::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CProgressBarUI::Render()
{
  	if(FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CProgressBarUI::Set_Progress(_float fProgress)
{
	if (fProgress > m_fMaxProgress)
		m_fProgress = m_fMaxProgress;

	m_fPreProgress = m_fProgress;
	m_fProgress = fProgress;
}

void CProgressBarUI::Set_MaxProgress(_float fMaxProgress)
{
	m_fMaxProgress = fMaxProgress;
}

HRESULT CProgressBarUI::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_ProgressBarUI"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CProgressBarUI::Bind_ShaderResources()
{
	if(FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Float("g_ProgressRate", m_fProgress / m_fMaxProgress)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Integer("g_iProgressBarTextureNum", m_iTextureIdx)))
		return E_FAIL;

	return S_OK;
}

CProgressBarUI* CProgressBarUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CProgressBarUI* pInstance = new CProgressBarUI(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : CProgressBarUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProgressBarUI::Clone(void* pArg)
{
	CProgressBarUI* pInstance = new CProgressBarUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : CProgressBarUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProgressBarUI::Free()
{
	__super::Free();
}
