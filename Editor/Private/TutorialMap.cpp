#include "TutorialMap.h"

#include "GameInstance.h"

CTutorialMap::CTutorialMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID)
	: CGameObject{ pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CTutorialMap::CTutorialMap(const CTutorialMap& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CTutorialMap::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTutorialMap::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	return S_OK;
}

void CTutorialMap::Priority_Update(_float fTimeDelta)
{
}

void CTutorialMap::Update(_float fTimeDelta)
{
}

void CTutorialMap::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CTutorialMap::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CTutorialMap::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_TutorialMap"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTutorialMap::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CTutorialMap* CTutorialMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID)
{
	CTutorialMap* pInstance = new CTutorialMap(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed: CTutorialMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTutorialMap::Clone(void* pArg)
{
	CTutorialMap* pInstance = new CTutorialMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed: CTutorialMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTutorialMap::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
