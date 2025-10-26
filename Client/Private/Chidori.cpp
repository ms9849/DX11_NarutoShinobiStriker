#include "Chidori.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"

#include "EffectContainer.h"
#include "EffectObject.h"
#include "ParticleObject.h"

CChidori::CChidori(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CSkill { pDevice, pContext, eObjectID }
{
}

CChidori::CChidori(const CChidori& rhs)
	: CSkill { rhs }
{
}

_bool CChidori::IsColliderActive()
{
	return m_pColliderCom->Get_Active();
}

void CChidori::Set_Visible(_bool bFlag)
{
	m_pEffectMain->Set_Visible(bFlag);
	m_pParticleMain->Set_Visible(bFlag);
}

HRESULT CChidori::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChidori::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CHIDORI_DESC* pDesc = static_cast<CHIDORI_DESC*>(pArg);
	m_pSocketMatrix = pDesc->pSocketMatrix;

	/* 치도리 차징 */
	CEffectContainer::EFFECT_CONTAINER_DESC EffectDesc;
	EffectDesc.IsBinary = true;
	EffectDesc.strFilePath = TEXT("../Bin/Resources/Effects/Chidori_eff.bin");

	m_pEffectMain = static_cast<CEffectContainer*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EffectContainer"),
		&EffectDesc));
	Safe_AddRef(m_pEffectMain);
	m_pEffectMain->Set_Blur(true);
	m_pGameInstance->Add_Clone_ToLayer(m_pEffectMain, m_pGameInstance->Get_LevelID(), TEXT("Layer_Effect"));

	CParticleObject::PARTICLE_LOAD_DESC ParticleDesc;
	ParticleDesc.eType = CParticleObject::PARTICLE_TYPE::EXPLOSION;
	ParticleDesc.strParticlePath = TEXT("../Bin/Resources/Particle/PurpleLightning_Particle.bin");

	m_pParticleMain = static_cast<CParticleObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"),
		&ParticleDesc));
	Safe_AddRef(m_pParticleMain);
	m_pGameInstance->Add_Clone_ToLayer(m_pParticleMain, m_pGameInstance->Get_LevelID(), TEXT("Layer_Particle"));


	///* 치도리 트레일 */
	//EffectDesc.IsBinary = true;
	//EffectDesc.strFilePath = TEXT("../Bin/Resources/Effects/Chidori_Trail_eff.bin");

	//m_pEffectTrail = static_cast<CEffectContainer*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EffectContainer"),
	//	&EffectDesc));
	//Safe_AddRef(m_pEffectTrail);
	//m_pGameInstance->Add_Clone_ToLayer(m_pEffectTrail, m_pGameInstance->Get_LevelID(), TEXT("Layer_Effect"));

	//m_pEffectTrail->Set_Visible(false);

	return S_OK;
}

void CChidori::Priority_Update(_float fTimeDelta)
{
}

void CChidori::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc >= 5.f)
		m_IsDead = true;

	///* 부모 행렬 적용 */
	//XMStoreFloat4x4(&m_CombinedWorldMatrix,
	//    XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pSocketMatrix) *
	//    XMLoadFloat4x4(CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform()->Get_WorldMatrixPtr()));

	///* 컴바인드 매트릭스 던져주면서 자연스럽게 크기도 따라가게 됨. */
	//m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));

	/* 부모 행렬 적용 */
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform()->Get_WorldMatrixPtr()));

	/* 컴바인드 매트릭스 던져주면서 자연스럽게 크기도 따라가게 됨. */
	m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
	m_pEffectMain->Set_ParentMatrix(XMMatrixScaling(1.9f, 1.9f, 1.9f) * XMLoadFloat4x4(&m_CombinedWorldMatrix));
	
	_vector vPosition = XMLoadFloat4((_float4*)(&m_CombinedWorldMatrix.m[3]));
	m_pParticleMain->Set_Position(vPosition);
}

void CChidori::Late_Update(_float fTimeDelta)
{
	CGameManager::GetInstance()->Add_Collider_ToCollision(TEXT("Player_Skill"),
		COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_CHIDORI, m_pColliderCom);

	//m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CChidori::Render()
{
	//if (FAILED(Bind_ShaderResources()))
	//    return E_FAIL;

	//_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	//for (size_t i = 0; i < iNumMeshes; i++)
	//{
	//    if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
	//        return E_FAIL;

	//    if (FAILED(m_pShaderCom->Begin(0)))
	//        return E_FAIL;

	//    if (FAILED(m_pModelCom->Render(i)))
	//        return E_FAIL;
	//}

	return S_OK;
}

HRESULT CChidori::Ready_Components()
{
	///* Com_Model */
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototoype_Component_Model_Rasengan"),
	//    TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//    return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC		AABBDesc{};

	AABBDesc.vSize = _float3(0.7f, 0.7f, 0.7f);


	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &AABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CChidori::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
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

CChidori* CChidori::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CChidori* pInstance = new CChidori(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : CChidori");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CChidori::Clone(void* pArg)
{
	CChidori* pInstance = new CChidori(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : CChidori");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChidori::Free()
{
	__super::Free();

	if(nullptr != m_pEffectMain)
		m_pEffectMain->Set_Dead(true);
	Safe_Release(m_pEffectMain);
	

	if (nullptr != m_pParticleMain)
		m_pParticleMain->Set_Dead(true);
	Safe_Release(m_pParticleMain);
	//Safe_Release(m_pEffectTrail);
}
