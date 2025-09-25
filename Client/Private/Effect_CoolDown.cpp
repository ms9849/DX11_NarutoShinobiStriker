#include "Effect_CoolDown.h"

#include "GameInstance.h"
#include "Pooling.h"

CEffect_CoolDown::CEffect_CoolDown(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CGameObject {  pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CEffect_CoolDown::CEffect_CoolDown(const CEffect_CoolDown& Prototype)
	: CGameObject { Prototype }
{
}

void CEffect_CoolDown::Set_Desc(void* pArg)
{
	EFFECT_COOLDOWN_DESC* pDesc = static_cast<EFFECT_COOLDOWN_DESC*>(pArg);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(pDesc->fX, pDesc->fY, 0.1f, 1.f));
	//m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(pDesc->fX - g_iWinSizeX / 2.f, -1.f * pDesc->fY + g_iWinSizeY / 2.f, 0.1f, 1.f));
}

HRESULT CEffect_CoolDown::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_CoolDown::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CEffect_CoolDown::Priority_Update(_float fTimeDelta)
{
}

void CEffect_CoolDown::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc >= 0.25f)
	{
		m_IsDead = true;
		m_fTimeAcc = 0.f;
		m_pTransformCom->Set_Scale(10.f, 10.f, 1.f);
	}
	else
		m_pTransformCom->Set_Scale(m_fTimeAcc * 400.f + 50.f, m_fTimeAcc * 400.f + 50.f, 1.f);
}

void CEffect_CoolDown::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CEffect_CoolDown::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_MASK))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_CoolDown::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SkillCoolDownEffect"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Orthogonal"),
		TEXT("Com_Orthogonal"), reinterpret_cast<CComponent**>(&m_pOrthogonalCom))))
		return E_FAIL;

	CPooling::POOLING_DESC Desc = {};
	Desc.strPoolingTag = TEXT("Effect_SkillCoolDown");

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Pooling"),
		reinterpret_cast<CComponent**>(&m_pPoolingCom), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_CoolDown::Bind_ShaderResources()
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

CEffect_CoolDown* CEffect_CoolDown::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CEffect_CoolDown* pInstance = new CEffect_CoolDown(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : Effect_CoolDown");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_CoolDown::Clone(void* pArg)
{
	CEffect_CoolDown* pInstance = new CEffect_CoolDown(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : Effect_CoolDown");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_CoolDown::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pOrthogonalCom);
	Safe_Release(m_pPoolingCom);
}