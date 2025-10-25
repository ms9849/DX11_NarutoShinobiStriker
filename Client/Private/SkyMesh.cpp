#include "SkyMesh.h"

#include "GameInstance.h"

CSkyMesh::CSkyMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CGameObject{ pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CSkyMesh::CSkyMesh(const CSkyMesh& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CSkyMesh::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkyMesh::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	/* 스카이 메시 세팅 */
	SKYMESH_DESC* pDesc = static_cast<SKYMESH_DESC*>(pArg);

	if (true == pDesc->IsNight)
		m_iShaderPassIdx = 2;

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();
	m_pTransformCom->Rotation(XMConvertToRadians(90.f), 0.f, 0.f);

	return S_OK;
}

void CSkyMesh::Priority_Update(_float fTimeDelta)
{
}

void CSkyMesh::Update(_float fTimeDelta)
{
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 0.1f);
}

void CSkyMesh::Late_Update(_float fTimeDelta)
{
	/* 카메라의 위치 따라가게끔, 회전은 없게. */
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(m_pGameInstance->Get_CamState(STATE::POSITION)));

	m_pGameInstance->Add_RenderGroup(RENDER::PRIORITY, this);
}

HRESULT CSkyMesh::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_Texture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(m_iShaderPassIdx)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CSkyMesh::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh_NonLight"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_SkyBox"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkyMesh::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CSkyMesh* CSkyMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CSkyMesh* pInstance = new CSkyMesh(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSkyBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkyMesh::Clone(void* pArg)
{
	CSkyMesh* pInstance = new CSkyMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CSkyBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkyMesh::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
