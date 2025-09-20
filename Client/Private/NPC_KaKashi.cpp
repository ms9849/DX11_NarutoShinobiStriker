#include "NPC_KaKashi.h"

#include "GameInstance.h"
#include "GameManager.h"

CNPC_KaKashi::CNPC_KaKashi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
	, m_pGameManager { CGameManager::GetInstance() } 
{
	Safe_AddRef(m_pGameManager);
}

CNPC_KaKashi::CNPC_KaKashi(const CNPC_KaKashi& rhs)
	: CGameObject { rhs }
	, m_pGameManager{ CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}

HRESULT CNPC_KaKashi::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC_KaKashi::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	m_pModelCom->Set_AnimIndex("Kakashi_etc_Tutorial_Loop", 1.f, false);
	m_eAnimState = ANIM_STATE::ANIM_IDLE;

	return S_OK;
}

void CNPC_KaKashi::Priority_Update(_float fTimeDelta)
{
}

void CNPC_KaKashi::Update(_float fTimeDelta)
{
	if (ANIM_STATE::ANIM_GREET == m_eAnimState)
	{
		m_pModelCom->Set_AnimIndex("Kakashi_etc_Tutorial_Loop", 1.f, false);
		m_eAnimState = ANIM_STATE::ANIM_IDLE;
	}

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CNPC_KaKashi::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CNPC_KaKashi::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_Texture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CNPC_KaKashi::Start_Dialog()
{
	/* 카메라도 NPC 전용으로 교체할 것. */
	m_pModelCom->Set_AnimIndex("KakashiNext_Reaction3", 1.f, true);
	m_eAnimState = ANIM_STATE::ANIM_GREET;
}

HRESULT CNPC_KaKashi::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_NPC_Kakashi"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_KaKashi::Bind_ShaderResources()
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

	/* 셰이더 & Constant anem 끌고 들어가서 다 세팅? */
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

CNPC_KaKashi* CNPC_KaKashi::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CNPC_KaKashi* pInstance = new CNPC_KaKashi(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : Kakashi");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CNPC_KaKashi::Clone(void* pArg)
{
	CNPC_KaKashi* pInstance = new CNPC_KaKashi(*this);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Clone Failed : Kakashi");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNPC_KaKashi::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pGameManager);
}
