#include "Mannequin.h"

#include "GameInstance.h"

CMannequin::CMannequin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID)
	: CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CMannequin::CMannequin(const CMannequin& rhs)
	: CGameObject { rhs }
{
}

HRESULT CMannequin::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMannequin::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	/*m_pModelCom->Set_AnimIndex(0);*/

	return S_OK;
}

void CMannequin::Priority_Update(_float fTimeDelta)
{
}

void CMannequin::Update(_float fTimeDelta)
{
	if(m_pGameInstance->Key_Down(DIK_0))
		m_pModelCom->Set_AnimIndex(0);

	if (m_pGameInstance->Key_Down(DIK_1))
		m_pModelCom->Set_AnimIndex(1);

	if (m_pGameInstance->Key_Down(DIK_2))
		m_pModelCom->Set_AnimIndex(2);

	//if (m_pGameInstance->Key_Down(DIK_3))
	//	m_pModelCom->Set_AnimIndex(3);

	//if (m_pGameInstance->Key_Down(DIK_4))
	//	m_pModelCom->Set_AnimIndex(4);


	m_pModelCom->Play_Animation(fTimeDelta);
}

void CMannequin::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CMannequin::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
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

HRESULT CMannequin::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EDIT), TEXT("Prototype_Component_Model_Test"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMannequin::Bind_ShaderResources()
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

CMannequin* CMannequin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID)
{
	CMannequin* pInstance = new CMannequin(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed ! : CMannequin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMannequin::Clone(void* pArg)
{
	CMannequin* pInstance = new CMannequin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed ! : CMannequin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMannequin::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
