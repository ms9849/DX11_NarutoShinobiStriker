#include "TimerUI.h"

#include "GameInstance.h"
#include "DecimalUI.h"

CTimerUI::CTimerUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CPanel { pDevice, pContext, eObjectID }
{
}

CTimerUI::CTimerUI(const CTimerUI& rhs)
	: CPanel { rhs }
{
}

HRESULT CTimerUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTimerUI::Initialize(void* pArg)
{
	m_iLeftTime = 599;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Decimals()))
		return E_FAIL;

	return S_OK;
}

void CTimerUI::Priority_Update(_float fTimeDelta)
{
}

void CTimerUI::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc >= 1.0f && m_iLeftTime > 0)
	{
		m_iLeftTime -= 1;
		m_fTimeAcc = 0.f;

		Calc_Timer();
	}
}

void CTimerUI::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CTimerUI::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTimerUI::Ready_Decimals()
{
	for (_int i = 0; i < 3; ++i)
	{
		CUIObject::UIOBJECT_DESC Desc;
		Desc.fSizeX = 90;
		Desc.fSizeY = 45;
		Desc.fY = m_fY + 7;
		Desc.fZ = m_fZ - 0.05f;

		switch (i)
		{
		case 0:
			Desc.fX = m_fX - 40;
			break;

		case 1:
			Desc.fX = m_fX + 7;
			break;

		case 2:
			Desc.fX = m_fX + 37;
			break;

		default:
			break;
		}

		CDecimalUI* pDecimal = static_cast<CDecimalUI*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_UI_Decimal"), &Desc));
		if (nullptr == pDecimal)
			return E_FAIL;

		m_pGameInstance->Add_Clone_ToLayer(pDecimal, ENUM_CLASS(LEVEL::LOGO), TEXT("Layer_UI"));
		m_Childs.push_back(pDecimal);
		Safe_AddRef(pDecimal);
	}

	Calc_Timer();

	return S_OK;
}

void CTimerUI::Calc_Timer()
{
	static_cast<CDecimalUI*>(m_Childs[0])->Set_CurrentIdx(m_iLeftTime / 60);
	static_cast<CDecimalUI*>(m_Childs[1])->Set_CurrentIdx(m_iLeftTime % 60 / 10);
	static_cast<CDecimalUI*>(m_Childs[2])->Set_CurrentIdx(m_iLeftTime % 60 % 10);
}

HRESULT CTimerUI::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_UI_Timer"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTimerUI::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pOrthogonalCom->Bind_ViewMatrix(m_pShaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_pOrthogonalCom->Bind_ProjMatrix(m_pShaderCom, "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CTimerUI* CTimerUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CTimerUI* pInstance = new CTimerUI(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : CTimerUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTimerUI::Clone(void* pArg)
{
	CTimerUI* pInstance = new CTimerUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : CTimerUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTimerUI::Free()
{
	__super::Free();
}
