#include "BossHPPanel.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "ProgressBarUI.h"

CBossHPPanel::CBossHPPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CPanel { pDevice, pContext, eObjectID }
{
}

CBossHPPanel::CBossHPPanel(const CBossHPPanel& rhs)
	: CPanel { rhs }
{
}

HRESULT CBossHPPanel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBossHPPanel::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_ProgressBar()))
		return E_FAIL;

	return S_OK;
}

void CBossHPPanel::Priority_Update(_float fTimeDelta)
{
}

void CBossHPPanel::Update(_float fTimeDelta)
{
}

void CBossHPPanel::Late_Update(_float fTimeDelta)
{
	if (false == m_IsVisible)
		return;

	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CBossHPPanel::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CBossHPPanel::Set_Progress(_float fProgress)
{
	static_cast<CProgressBarUI*>(m_Childs[0])->Set_Progress(fProgress);
}

void CBossHPPanel::Set_MaxProgress(_float fMaxProgress)
{
	static_cast<CProgressBarUI*>(m_Childs[0])->Set_MaxProgress(fMaxProgress);
}

HRESULT CBossHPPanel::Ready_ProgressBar()
{
	UIOBJECT_DESC Desc = CUIObject::CreateDesc(m_fX, m_fY, m_fZ - 0.05f, g_iWinSizeX - 300, 10, 1, 0.f);

	CProgressBarUI* pProgressBar = static_cast<CProgressBarUI*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC),
		TEXT("Prototype_GameObject_ProgressBarUI"), &Desc));

	/* 추후 레벨 수정해야함 */
	m_pGameInstance->Add_Clone_ToLayer(pProgressBar, ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Layer_UI"));
	m_Childs.push_back(pProgressBar);
	Safe_AddRef(pProgressBar);

	return S_OK;
}

HRESULT CBossHPPanel::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BossHPPanel"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossHPPanel::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

CBossHPPanel* CBossHPPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CBossHPPanel* pInstance = new CBossHPPanel(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed! : CBossHPPanel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBossHPPanel::Clone(void* pArg)
{
	CBossHPPanel* pInstance = new CBossHPPanel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed! : CBossHPPanel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossHPPanel::Free()
{
	__super::Free();
}
