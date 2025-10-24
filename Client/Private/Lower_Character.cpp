#include "Lower_Character.h"

#include "GameInstance.h"
#include "Trail.h"

CLower_Character::CLower_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CParts_Character { pDevice, pContext, eObjectID }
{
}

CLower_Character::CLower_Character(const CLower_Character& Prototype)
	: CParts_Character { Prototype }
{
}

HRESULT CLower_Character::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLower_Character::Initialize(void* pArg)
{
	LOWER_PLAYER_DESC* pDesc = static_cast<LOWER_PLAYER_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CTrail::TRAIL_DESC Desc;
	XMStoreFloat4(&Desc.vHighPosition, XMVectorSet(-0.02f, 0.f, 0.f, 1.f));
	XMStoreFloat4(&Desc.vLowPosition, XMVectorSet(0.f, 0.f, 0.02f, 1.f));
	Desc.strTrailTextureTag = TEXT("Prototype_Component_Texture_FootTrail_Blue");
	//Desc.strTrailTextureTag = TEXT("Prototype_Component_Texture_SwordTrail");

	m_pFootTrail[0] = static_cast<CTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_FootTrail"), &Desc));
	m_pFootTrail[1] = static_cast<CTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_FootTrail"), &Desc));

	return S_OK;
}

void CLower_Character::Priority_Update(_float fTimeDelta)
{
}

void CLower_Character::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;
	/* 부모 행렬 적용 */
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));


	if (m_fTimeAcc >= 0.01f)
	{
		m_pFootTrail[0]->Update_Trail(XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("LeftFoot")) * XMLoadFloat4x4(&m_CombinedWorldMatrix), true);
		m_pFootTrail[1]->Update_Trail(XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("RightFoot")) * XMLoadFloat4x4(&m_CombinedWorldMatrix), true);
		m_fTimeAcc = 0.f;
	}
}

void CLower_Character::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

	m_pFootTrail[0]->Late_Update(fTimeDelta);
	m_pFootTrail[1]->Late_Update(fTimeDelta);
}

HRESULT CLower_Character::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
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

HRESULT CLower_Character::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CLower_Character::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), m_strModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLower_Character::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CLower_Character* CLower_Character::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CLower_Character* pInstance = new CLower_Character(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLower_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLower_Character::Clone(void* pArg)
{
	CLower_Character* pInstance = new CLower_Character(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLower_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLower_Character::Free()
{
	__super::Free();

	Safe_Release(m_pFootTrail[0]);
	Safe_Release(m_pFootTrail[1]);
}
