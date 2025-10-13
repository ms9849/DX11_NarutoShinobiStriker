#include "EffectObject.h"

#include "GameInstance.h"

CEffectObject::CEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID)
    : CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID)}
{
}

CEffectObject::CEffectObject(const CEffectObject& rhs) 
    : CGameObject { rhs }
{
}

HRESULT CEffectObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffectObject::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

    return S_OK;
}

void CEffectObject::Priority_Update(_float fTimeDelta)
{
}

void CEffectObject::Update(_float fTimeDelta)
{
}

void CEffectObject::Late_Update(_float fTimeDelta)
{
    /* 문제없나..?*/
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CEffectObject::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		/* 바인드 머테리얼이 아니라 내가 들고 있는 텍스쳐를 선택해서 바인딩 해줘야지. */
		if (FAILED(m_pDiffuseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iDiffuseTextureIdx)))
			return E_FAIL;

		if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_iMaskTextureIdx)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CEffectObject::Set_Desc(void* pArg)
{
	EFFECT_OBJECT_DESC* pDesc = static_cast<EFFECT_OBJECT_DESC*>(pArg);

	if(-1 != pDesc->iTextureNum)
		m_iDiffuseTextureIdx = pDesc->iTextureNum;

	if (-1 != pDesc->iMaskTextureNum)
		m_iMaskTextureIdx = pDesc->iMaskTextureNum;

	if (TEXT("") != pDesc->strMaskTextureTag)
	{
		Safe_Release(m_p)
	}


}

HRESULT CEffectObject::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Shader_VtxEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_EffectModel_Test"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_DiffuseTexture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Effect"),
		TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pDiffuseTextureCom))))
		return E_FAIL;

	/* Com_MaskTexture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Effect_Mask"),
		TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectObject::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CEffectObject* CEffectObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID)
{
	CEffectObject* pInstance = new CEffectObject(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : EffectObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffectObject::Clone(void* pArg)
{
	CEffectObject* pInstance = new CEffectObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CEffectObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectObject::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pDiffuseTextureCom);
	Safe_Release(m_pMaskTextureCom);
}
