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
	Play_Sprite(fTimeDelta);
	Check_LifeTime(fTimeDelta);
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
		if (FAILED(m_pShaderCom->Bind_RawValue("g_iNumWidth", &m_iNumWidth, sizeof(_uint))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_iNumHeight", &m_iNumWidth, sizeof(_uint))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDeltaU", &m_fDeltaU, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDeltaV", &m_fDeltaV, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_iCurrentIdx", &m_iCurrentIdx, sizeof(_uint))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fLifeTimeAcc", &m_fLifeTimeAcc, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fLifeTime", &m_fLifeTime, sizeof(_float))))
			return E_FAIL;

		/* 바인드 머테리얼이 아니라 내가 들고 있는 텍스쳐를 선택해서 바인딩 해줘야지. */
		if (FAILED(m_pDiffuseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iDiffuseTextureIdx)))
			return E_FAIL;

		if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_iMaskTextureIdx)))
			return E_FAIL;

		if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", m_iNoiseTextureIdx)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CEffectObject::Play_Sprite(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc >= m_fFrameTime)
	{
		m_fTimeAcc = 0.f;
		m_iCurrentIdx++;

		if (m_iCurrentIdx >= m_iMaxIdx)
			m_iCurrentIdx = 0;
	}
}

void CEffectObject::Check_LifeTime(_float fTimeDelta)
{
	m_fLifeTime += fTimeDelta;

	if (m_fLifeTimeAcc >= m_fLifeTime)
	{
		m_fLifeTimeAcc = 0.f;
		//사망처리 해줘야됨 원래
	}
}

void CEffectObject::Set_Desc(void* pArg)
{
	EFFECT_OBJECT_DESC* pDesc = static_cast<EFFECT_OBJECT_DESC*>(pArg);

	m_iNumHeight = pDesc->iNumHeight;
	m_iNumWidth = pDesc->iNumWidth;
	m_iCurrentIdx = pDesc->iCurrentIdx;
	m_fFrameTime = pDesc->fFrameTime;
	m_fLifeTime = pDesc->fFrameTime * m_iNumHeight * m_iNumWidth;

	if (m_iNumHeight * m_iNumWidth > 0)
		m_iMaxIdx = m_iNumHeight * m_iNumWidth - 1;
	else
		m_iMaxIdx = 0;

	if(-1 != pDesc->iTextureNum)
		m_iDiffuseTextureIdx = pDesc->iTextureNum;

	if (-1 != pDesc->iMaskTextureNum)
		m_iMaskTextureIdx = pDesc->iMaskTextureNum;

	if (-1 != pDesc->iNoiseTextureNum)
		m_iNoiseTextureIdx = pDesc->iNoiseTextureNum;

	/* Com_DiffuseTexture */
	if (TEXT("") != pDesc->strTextureTag)
	{
		Safe_Release(m_pDiffuseTextureCom);

		auto iter = m_Components.find(TEXT("Com_DiffuseTexture"));
		m_Components.erase(iter);
		Safe_Release(iter->second);

		__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), pDesc->strTextureTag,
			TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom));
	}

	/* Com_MaskTexture */
	if (TEXT("") != pDesc->strMaskTextureTag)
	{
		Safe_Release(m_pMaskTextureCom);

		auto iter = m_Components.find(TEXT("Com_MaskTexture"));
		m_Components.erase(iter);
		Safe_Release(iter->second);

		__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), pDesc->strMaskTextureTag,
			TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom));
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
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Effect_Diffuse"),
		TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pDiffuseTextureCom))))
		return E_FAIL;

	/* Com_MaskTexture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Effect_Mask"),
		TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
		return E_FAIL;

	/* Com_NoiseTexture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Effect_Noise"),
		TEXT("Com_NoiseTexture"), reinterpret_cast<CComponent**>(&m_pNoiseTextureCom))))
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
	Safe_Release(m_pNoiseTextureCom);
}
