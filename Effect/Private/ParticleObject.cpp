#include "ParticleObject.h"

#include "GameInstance.h"

CParticleObject::CParticleObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID)
	: CGameObject{ pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CParticleObject::CParticleObject(const CParticleObject& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CParticleObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticleObject::Initialize(void* pArg)
{
	PARTICLE_OBJECT_DESC* pDesc = static_cast<PARTICLE_OBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_VIBuffer(pArg)))
		return E_FAIL;

	m_eType = pDesc->eType;
	m_vMainColor = pDesc->vMainColor;
	m_vSubColor = pDesc->vSubColor;
	m_iNumHeight = pDesc->iNumHeight;
	m_iNumWidth = pDesc->iNumWidth;
	m_fFrameTime = pDesc->fFrameTime;
	m_iCurrentIdx = pDesc->iCurrentIdx;
	m_iShaderPassIdx = pDesc->iShaderPassIdx;
	m_iDiffuseTextureIdx = pDesc->iDiffuseTextureNum;
	m_iMaskTextureIdx = pDesc->iMaskTextureNum;
	m_iNoiseTextureIdx = pDesc->iNoiseTextureNum;
	m_iMaxIdx = m_iNumHeight * m_iNumWidth - 1;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CParticleObject::Priority_Update(_float fTimeDelta)
{
}

void CParticleObject::Update(_float fTimeDelta)
{
	switch (m_eType)
	{
	case PARTICLE_TYPE::EXPLOSION:
		m_pVIBufferCom->Explosion(fTimeDelta);
		break;

	case PARTICLE_TYPE::DROP:
		m_pVIBufferCom->Drop(fTimeDelta);
		break;

	case PARTICLE_TYPE::FLOAT_DROP:
		m_pVIBufferCom->FloatAndDrop(fTimeDelta);
		break;

	case PARTICLE_TYPE::EXPLOSION_FLOAT:
		m_pVIBufferCom->ExplosionAndFloat(fTimeDelta);
		break;

	case PARTICLE_TYPE::EXPLOSION_NON_UV:
		m_pVIBufferCom->Explosion(fTimeDelta);
		break;

	default:
		break;
	}

	if (true == m_pVIBufferCom->IsAllDead())
		m_IsDead = true;	

	Play_Sprite(fTimeDelta);
}

void CParticleObject::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::BLUR, this);
}

HRESULT CParticleObject::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(ENUM_CLASS(m_eType));

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CParticleObject::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Shader_VtxPointParticle"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_DiffuseTexture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Particle"),
		TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pDiffuseTexCom))))
		return E_FAIL;

	/* Com_MaskTexture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Effect_Mask"),
		TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTexCom))))
		return E_FAIL;

	/* Com_NoiseTexture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Effect_Noise"),
		TEXT("Com_NoiseTexture"), reinterpret_cast<CComponent**>(&m_pNoiseTexCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticleObject::Ready_VIBuffer(void* pArg)
{
	/* Com_VIBuffer */
	PARTICLE_OBJECT_DESC* pParticleDesc = static_cast<PARTICLE_OBJECT_DESC*>(pArg);
	CVIBuffer_Point_Instance::POINT_INSTANCE_DESC Desc;

	Desc.isLoop = pParticleDesc->isLoop;
	Desc.vPivot = pParticleDesc->vPivot;
	Desc.vSpeed = pParticleDesc->vSpeed;
	Desc.vRotation = pParticleDesc->vRotation;
	Desc.vLifeTime = pParticleDesc->vLifeTime;
	Desc.iNumInstance = pParticleDesc->iNumInstance;
	/* 사이즈 입력 안받음 */
	Desc.vSize = pParticleDesc->vSize;
	Desc.vCenter = pParticleDesc->vCenter;
	Desc.vRange = pParticleDesc->vRange;

	m_pVIBufferCom = CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &Desc);
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;
	m_pVIBufferCom->Initialize(nullptr);

	return S_OK;
}

void CParticleObject::Play_Sprite(_float fTimeDelta)
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

HRESULT CParticleObject::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pDiffuseTexCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iDiffuseTextureIdx)))
		return E_FAIL;

	if (FAILED(m_pMaskTexCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_iMaskTextureIdx)))
		return E_FAIL;

	if (FAILED(m_pNoiseTexCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", m_iNoiseTextureIdx)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamState(STATE::POSITION), sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMainColor", &m_vMainColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vSubColor", &m_vSubColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iCurrentIdx", &m_iCurrentIdx, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iNumWidth", &m_iNumWidth, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iNumHeight", &m_iNumWidth, sizeof(_uint))))
		return E_FAIL;

	return S_OK;
}

CParticleObject* CParticleObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID)
{
	CParticleObject* pInstance = new CParticleObject(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : Particle Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParticleObject::Clone(void* pArg)
{
	CParticleObject* pInstance = new CParticleObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : Particle Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticleObject::Free()
{
	__super::Free();

	Safe_Release(m_pDiffuseTexCom);
	Safe_Release(m_pMaskTexCom);
	Safe_Release(m_pNoiseTexCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
