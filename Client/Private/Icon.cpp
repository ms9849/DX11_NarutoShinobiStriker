#include "Icon.h"
#include "GameManager.h"
#include "GameInstance.h"

CIcon::CIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CGameObject { pDevice, pContext, eObjectID }
{
}

CIcon::CIcon(const CIcon& rhs) 
	: CGameObject{ rhs }
{
}

HRESULT CIcon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CIcon::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	ENEMY_HPBAR_DESC* pDesc = static_cast<ENEMY_HPBAR_DESC*>(pArg);
	m_pTargetTransform = pDesc->pTargetTransform;
	m_iTextureIdx = pDesc->iTextureIdx;

	Safe_AddRef(m_pTargetTransform);

	return S_OK;
}

void CIcon::Priority_Update(_float fTimeDelta)
{
}

void CIcon::Update(_float fTimeDelta)
{
}

void CIcon::Late_Update(_float fTimeDelta)
{
	/* 업데이트 어느 순간에 돌지 모르니까. */
	m_pTransformCom->Set_State(STATE::POSITION, m_pTargetTransform->Get_State(STATE::POSITION) + XMVectorSet(0.f, 1.75f, 0.f, 0.f));
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CIcon::Render()
{
	/* 카메라랑 같은 시점 바라보게끔 설정 */
	_float4x4 CameraWorld = *m_pGameInstance->Get_PipeLine_InverseFloat4x4(D3DTS::VIEW);

	m_pTransformCom->Set_State(STATE::RIGHT, *reinterpret_cast<_vector*>(&CameraWorld.m[0]));
	m_pTransformCom->Set_State(STATE::UP, *reinterpret_cast<_vector*>(&CameraWorld.m[1]));
	m_pTransformCom->Set_State(STATE::LOOK, *reinterpret_cast<_vector*>(&CameraWorld.m[2]));

	m_pTransformCom->Set_Scale(0.4f, 0.1f, 1.f);

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_ENEMYHPBAR))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CIcon::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_EnemyHPBar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CIcon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIdx)))
		return E_FAIL;

	return S_OK;
}

CIcon* CIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CIcon* pInstance = new CIcon(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : CIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CIcon* CIcon::Clone(void* pArg)
{
	CIcon* pInstance = new CIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : CIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIcon::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTargetTransform);
}
