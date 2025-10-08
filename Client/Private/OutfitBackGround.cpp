#include "OutfitBackGround.h"

#include "GameInstance.h"

COutfitBackGround::COutfitBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

COutfitBackGround::COutfitBackGround(const COutfitBackGround& rhs)
	: CGameObject { rhs }
{
}

HRESULT COutfitBackGround::Initialize_Prototype()
{
	return S_OK;
}

HRESULT COutfitBackGround::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(g_iWinSizeX, g_iWinSizeY, 1.f);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.05f, 1.f));

	return S_OK;
}

void COutfitBackGround::Priority_Update(_float fTimeDelta)
{
}

void COutfitBackGround::Update(_float fTimeDelta)
{
}

void COutfitBackGround::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::PRIORITY, this);
}

HRESULT COutfitBackGround::Render()
{
	/* 셰이더에 리소스 바인딩하고 */
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 정해진 Pass로 셰이더 시작*/
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX_IDX::OUTFIT_BACKGROUND))))
		return E_FAIL;

	/* 버퍼 바인딩 */
	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	/* 출력 */
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT COutfitBackGround::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::OUTFITSELECT), TEXT("Prototype_Component_Texture_OutfitBackGround"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Orthogonal"),
		TEXT("Com_Orthogonal"), reinterpret_cast<CComponent**>(&m_pOrthogonalCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT COutfitBackGround::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pOrthogonalCom->Bind_ViewMatrix(m_pShaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_pOrthogonalCom->Bind_ProjMatrix(m_pShaderCom, "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

COutfitBackGround* COutfitBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	COutfitBackGround* pInstance = new COutfitBackGround(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : OutfitBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* COutfitBackGround::Clone(void* pArg)
{
	COutfitBackGround* pInstance = new COutfitBackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : OutfitBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COutfitBackGround::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pOrthogonalCom);
	Safe_Release(m_pVIBufferCom);
}
