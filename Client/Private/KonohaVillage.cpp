#include "KonohaVillage.h"
#include "GameInstance.h"

#include "Mesh.h"

CKonohaVillage::CKonohaVillage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CGameObject{ pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CKonohaVillage::CKonohaVillage(const CKonohaVillage& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CKonohaVillage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKonohaVillage::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.7f, 0.f, 1.f));
	m_pGameInstance->Add_StaticShadow(this);

	return S_OK;
}

void CKonohaVillage::Priority_Update(_float fTimeDelta)
{
}

void CKonohaVillage::Update(_float fTimeDelta)
{
	for (_uint i = 0; i < ENUM_CLASS(KONOHA_VILLAGE_SECTION::END); ++i)
	{
		m_NavigationComs[i]->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
	}
}

void CKonohaVillage::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CKonohaVillage::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		_uint iShaderPassIdx = { 0 };
		_wstring strMeshName = m_pModelCom->Get_Mesh(i)->Get_Name();
		if (
			TEXT("SM_ENV_KNVLLG_ground_01.md") == strMeshName || 
			TEXT("SM_ENV_KNVLLG_ground_02.md") == strMeshName || 
			TEXT("SM_ENV_KNVLLG_ground_03.md") == strMeshName || 
			TEXT("SM_ENV_KNVLLG_ground_04.md") == strMeshName || 
			TEXT("SM_ENV_KNVLLG_ground_05.md") == strMeshName ||
			TEXT("SM_ENV_KNVLLG_ground_06.md") == strMeshName ||
			TEXT("SM_ENV_KNVLLG_ground_07.md") == strMeshName ||
			TEXT("SM_ENV_KNVLLG_ground_08.md") == strMeshName ||
			TEXT("SM_ENV_KNVLLG_ground_09.md") == strMeshName ||
			TEXT("SM_ENV_KNVLLG_ground_10.md") == strMeshName ||
			TEXT("SM_ENV_KNVLLG_ground_11.md") == strMeshName ||
			TEXT("SM_ENV_KNVLLG_ground_12.md") == strMeshName ||
			TEXT("SM_ENV_KNVLLG_ground_13.md") == strMeshName ||
			TEXT("SM_ENV_KNVLLG_ground_14.md") == strMeshName
			)
			iShaderPassIdx = 3;

		if (FAILED(m_pShaderCom->Begin(iShaderPassIdx)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	for (_uint i = 0; i < ENUM_CLASS(KONOHA_VILLAGE_SECTION::END); ++i)
	{
		m_NavigationComs[i]->Render();
	}
#endif

	return S_OK;
}
HRESULT CKonohaVillage::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Model_KonohaVillage"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Navigation */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Navigation_KonohaVillage_1"),
		TEXT("Com_Navigation_1"), reinterpret_cast<CComponent**>(&m_NavigationComs[0]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Navigation_KonohaVillage_2"),
		TEXT("Com_Navigation_2"), reinterpret_cast<CComponent**>(&m_NavigationComs[1]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Navigation_KonohaVillage_3"),
		TEXT("Com_Navigation_3"), reinterpret_cast<CComponent**>(&m_NavigationComs[2]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Navigation_KonohaVillage_4"),
		TEXT("Com_Navigation_4"), reinterpret_cast<CComponent**>(&m_NavigationComs[3]))))
		return E_FAIL;

	return S_OK;
}

HRESULT CKonohaVillage::Bind_ShaderResources()
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

CKonohaVillage* CKonohaVillage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CKonohaVillage* pInstance = new CKonohaVillage(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed: KonohaVillage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CKonohaVillage::Clone(void* pArg)
{
	CKonohaVillage* pInstance = new CKonohaVillage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed: KonohaVillage");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CKonohaVillage::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	for (_uint i = 0; i < ENUM_CLASS(KONOHA_VILLAGE_SECTION::END); ++i)
	{
		Safe_Release(m_NavigationComs[i]);
	} 
}
