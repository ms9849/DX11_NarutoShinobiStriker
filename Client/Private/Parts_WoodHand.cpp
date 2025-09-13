#include "Parts_WoodHand.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"

CParts_WoodHand::CParts_WoodHand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CPartObject{ pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CParts_WoodHand::CParts_WoodHand(const CParts_WoodHand& rhs)
	: CPartObject{ rhs }
{
}

void CParts_WoodHand::Play_Animation(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CParts_WoodHand::Attack()
{
	if (true == m_IsAttack)
		return;

	if (m_eHandType == HAND_TYPE::LEFT)
		m_pModelCom->Set_AnimIndex("WoodHandShort_L_Ninjutsu_LaughingMonk_End", 1.f, true, 0.15f, false);

	else if (m_eHandType == HAND_TYPE::RIGHT)
		m_pModelCom->Set_AnimIndex("WoodHandShort_R_Ninjutsu_LaughingMonk_End", 1.f, true, 0.15f, false);

	m_IsAttack = true;
}

HRESULT CParts_WoodHand::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParts_WoodHand::Initialize(void* pArg)
{
	PARTS_WOODHAND_DESC* pDesc = static_cast<PARTS_WOODHAND_DESC*>(pArg);

	m_eHandType = pDesc->eHandType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	_vector vPlayerRight = CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform()->Get_State(STATE::RIGHT);
	if (m_eHandType == HAND_TYPE::LEFT)
	{
		m_pModelCom->Set_AnimIndex("WoodHandShort_L_Ninjutsu_LaughingMonk_Start", 1.f, false, 0.15f, true);
		m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION));
	}

	else  if (m_eHandType == HAND_TYPE::RIGHT)
	{
		m_pModelCom->Set_AnimIndex("WoodHandShort_R_Ninjutsu_LaughingMonk_Start", 1.f, false, 0.15f, true);
		m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION));
	}

	return S_OK;
}

void CParts_WoodHand::Priority_Update(_float fTimeDelta)
{
}

void CParts_WoodHand::Update(_float fTimeDelta)
{
	/* 부모 행렬 적용 */
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

	_bool IsFinished = { false };

	if(false == m_IsAttackFinished)
		IsFinished = m_pModelCom->Play_Animation(fTimeDelta);

	/* 공격 모션 끝나면 다시 돌아가기 */
	if (true == IsFinished && true == m_IsAttack)
	{
		m_IsAttackFinished = true;
	}
}

void CParts_WoodHand::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CParts_WoodHand::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CParts_WoodHand::Ready_Components()
{
	/* Com_Model */
	if (m_eHandType == HAND_TYPE::LEFT)
	{
		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_WoodHand_L"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}
	else if (m_eHandType == HAND_TYPE::RIGHT)
	{
		if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_WoodHand_R"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}
	
	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParts_WoodHand::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
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

CParts_WoodHand* CParts_WoodHand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CParts_WoodHand* pInstance = new CParts_WoodHand(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : Parts Wood Hand");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParts_WoodHand::Clone(void* pArg)
{
	CParts_WoodHand* pInstance = new CParts_WoodHand(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : Parts Wood Hand");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParts_WoodHand::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
