#include "Props.h"

#include "GameInstance.h"

CProps::CProps(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CProps::CProps(const CProps& rhs)
	: CGameObject { rhs }
{
}

HRESULT CProps::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProps::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	PROP_DESC* pDesc = static_cast<PROP_DESC*>(pArg);

	if (nullptr == pDesc)
		return S_OK;

	m_iMeshIdx = pDesc->iMeshIdx;
	m_iShaderPassIdx = pDesc->iShaderPassIdx;

	return S_OK;
}

void CProps::Priority_Update(_float fTimeDelta)
{
}

void CProps::Update(_float fTimeDelta)
{
}

void CProps::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CProps::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pModelCom->Bind_Material(m_iMeshIdx, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iShaderPassIdx)))
		return E_FAIL;

	if (FAILED(m_pModelCom->Render(m_iMeshIdx)))
		return E_FAIL;

	return S_OK;
}

HRESULT CProps::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Props"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CProps::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CProps* CProps::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CProps* pInstance = new CProps(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : Props");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProps::Clone(void* pArg)
{
	CProps* pInstance = new CProps(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Props");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProps::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
