#include "Enemy_HPBar.h"

#include "GameInstance.h"
#include "Icon.h"

CEnemy_HPBar::CEnemy_HPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID)}
{
}

CEnemy_HPBar::CEnemy_HPBar(const CEnemy_HPBar& rhs)
	: CGameObject { rhs }
{
}


void CEnemy_HPBar::Set_HP(_float fCurrentHP, _float fMaxHP)
{
	m_fCurrentHp = fCurrentHP;
	m_fMaxHp = fMaxHP;
}

HRESULT CEnemy_HPBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEnemy_HPBar::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	ENEMY_HPBAR_DESC* pDesc = static_cast<ENEMY_HPBAR_DESC*>(pArg);
	m_pTargetTransform = pDesc->pTargetTransform;
	Safe_AddRef(m_pTargetTransform);

	return S_OK;
}

void CEnemy_HPBar::Priority_Update(_float fTimeDelta)
{
}

void CEnemy_HPBar::Update(_float fTimeDelta)
{
}

void CEnemy_HPBar::Late_Update(_float fTimeDelta)
{
	/* 업데이트 어느 순간에 돌지 모르니까. */
	m_pTransformCom->Set_State(STATE::POSITION, m_pTargetTransform->Get_State(STATE::POSITION) + XMVectorSet(0.f, 1.75f, 0.f, 0.f));
	m_pGameInstance->Add_RenderGroup(RENDER::WORLD_UI, this);
}

HRESULT CEnemy_HPBar::Render()
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

	if(FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy_HPBar::Ready_Components()
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

HRESULT CEnemy_HPBar::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CurrentHP", &m_fCurrentHp, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaxHP", &m_fMaxHp, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CEnemy_HPBar* CEnemy_HPBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CEnemy_HPBar* pInstance = new CEnemy_HPBar(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : ENEMY HPBAR");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEnemy_HPBar* CEnemy_HPBar::Clone(void* pArg)
{
	CEnemy_HPBar* pInstance = new CEnemy_HPBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : ENEMY HPBAR");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy_HPBar::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTargetTransform);
}
