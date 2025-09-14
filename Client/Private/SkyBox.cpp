#include "SkyBox.h"

#include "GameInstance.h"

CSkyBox::CSkyBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CGameObject{ pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CSkyBox::CSkyBox(const CSkyBox& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CSkyBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkyBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	D3D11_RASTERIZER_DESC RasterizeDesc = {};
	RasterizeDesc.FillMode = D3D11_FILL_SOLID;
	RasterizeDesc.CullMode = D3D11_CULL_NONE;
	RasterizeDesc.FrontCounterClockwise = FALSE;
	RasterizeDesc.DepthClipEnable = TRUE;

	m_pDevice->CreateRasterizerState(&RasterizeDesc, &m_pSkyBoxRasterizeState);

	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {};
	DepthStencilDesc.DepthEnable = TRUE;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 깊이 버퍼에 쓰기 금지
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	m_pDevice->CreateDepthStencilState(&DepthStencilDesc, &m_pSkyBoxDepthStencilState);


	return S_OK;
}

void CSkyBox::Priority_Update(_float fTimeDelta)
{
}

void CSkyBox::Update(_float fTimeDelta)
{
}

void CSkyBox::Late_Update(_float fTimeDelta)
{
	/* 카메라의 위치 따라가게끔, 회전은 없게. */
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(m_pGameInstance->Get_CamState(STATE::POSITION)));

	m_pGameInstance->Add_RenderGroup(RENDER::PRIORITY, this);
}

HRESULT CSkyBox::Render()
{
	m_pContext->RSGetState(&m_pOldRasterizeState);
	m_pContext->RSSetState(m_pSkyBoxRasterizeState);

	m_pContext->OMGetDepthStencilState(&m_pOldDepthStencilState, &m_iOldStencilRef);
	m_pContext->OMSetDepthStencilState(m_pSkyBoxDepthStencilState, 0);

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

	m_pContext->RSSetState(m_pOldRasterizeState);
	m_pContext->OMSetDepthStencilState(m_pOldDepthStencilState, m_iOldStencilRef);

	return S_OK;
}

HRESULT CSkyBox::Ready_Components()
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

HRESULT CSkyBox::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CSkyBox* CSkyBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CSkyBox* pInstance = new CSkyBox(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSkyBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkyBox::Clone(void* pArg)
{
	CSkyBox* pInstance = new CSkyBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CSkyBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkyBox::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pSkyBoxRasterizeState);
	Safe_Release(m_pSkyBoxDepthStencilState);
}
