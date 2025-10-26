#include "BigShark.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "EffectContainer.h"
#include "EffectObject.h"
#include "ParticleObject.h"

CBigShark::CBigShark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CSkill { pDevice, pContext, eObjectID }
{
}

CBigShark::CBigShark(const CBigShark& rhs)
	: CSkill { rhs }
{
}

HRESULT CBigShark::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBigShark::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	BIGSHARK_DESC* pDesc = static_cast<BIGSHARK_DESC*>(pArg);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
	m_pTransformCom->LookAt(m_pTransformCom->Get_State(STATE::POSITION) - XMLoadFloat3(&pDesc->vLook));

	/* ¾Æ±â»ó¾î¶Ñ·ç·ç¶Ñ·ç */
	CEffectContainer::EFFECT_CONTAINER_DESC EffectDesc;
	EffectDesc.IsBinary = true;
	EffectDesc.strFilePath = TEXT("../Bin/Resources/Effects/BigSharkBomb_eff.bin");


	m_pEffectMain = static_cast<CEffectContainer*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EffectContainer"),
		&EffectDesc));
	Safe_AddRef(m_pEffectMain);
	m_pEffectMain->Set_Blur(true);

	m_pGameInstance->Add_Clone_ToLayer(m_pEffectMain, m_pGameInstance->Get_LevelID(), TEXT("Layer_Effect"));

	CParticleObject::PARTICLE_LOAD_DESC ParticleDesc;
	ParticleDesc.eType = CParticleObject::PARTICLE_TYPE::FLOAT_DROP;
	ParticleDesc.strParticlePath = TEXT("../Bin/Resources/Particle/Shark_1_Particle.bin");

	m_pParticleMain = static_cast<CParticleObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"),
		&ParticleDesc));
	Safe_AddRef(m_pParticleMain);
	m_pGameInstance->Add_Clone_ToLayer(m_pParticleMain, m_pGameInstance->Get_LevelID(), TEXT("Layer_Particle"));

	ParticleDesc.strParticlePath = TEXT("../Bin/Resources/Particle/Shark_2_Particle.bin");

	m_pParticleSub = static_cast<CParticleObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"),
		&ParticleDesc));
	Safe_AddRef(m_pParticleSub);
	m_pGameInstance->Add_Clone_ToLayer(m_pParticleSub, m_pGameInstance->Get_LevelID(), TEXT("Layer_Particle"));

	return S_OK;
}

void CBigShark::Priority_Update(_float fTimeDelta)
{
}

void CBigShark::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc >= m_fLifeTime)
		m_IsDead = true;

	m_pTransformCom->Go_Backward(fTimeDelta, nullptr);

	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	m_pEffectMain->Set_ParentMatrix(XMMatrixTranslation(0.f, 0.02f, 0.f) * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));


	m_pParticleMain->Set_Position(m_pTransformCom->Get_State(STATE::POSITION));
	m_pParticleSub->Set_Position(m_pTransformCom->Get_State(STATE::POSITION));
}

void CBigShark::Late_Update(_float fTimeDelta)
{
	CGameManager::GetInstance()->Add_Collider_ToCollision(TEXT("Player_Skill"),
		COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_BIGSHARK, m_pColliderCom);

	//m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	//m_pGameInstance->Add_RenderGroup(RENDER::BLUR, this);
#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CBigShark::Render()
{
	return S_OK;
}

HRESULT CBigShark::Ready_Components()
{
	///* Com_Model */
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototoype_Component_Model_BigShark"),
	//    TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//    return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC		OBBDesc{};

	OBBDesc.vSize = { 5.f, 1.f, 5.f };
	OBBDesc.vCenter = { 0.f, 0.f, 0.f };

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBigShark::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamState(STATE::POSITION), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CBigShark* CBigShark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CBigShark* pInstance = new CBigShark(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : CBigShark");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBigShark::Clone(void* pArg)
{
	CBigShark* pInstance = new CBigShark(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : CBigShark");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBigShark::Free()
{
	__super::Free();
	

	if (nullptr != m_pEffectMain)
		m_pEffectMain->Set_Dead(true);

	if (nullptr != m_pParticleMain)
		m_pParticleMain->Set_Dead(true);

	if (nullptr != m_pParticleSub)
		m_pParticleSub->Set_Dead(true);

	Safe_Release(m_pEffectMain); 
	Safe_Release(m_pParticleMain);
	Safe_Release(m_pParticleSub);
}
