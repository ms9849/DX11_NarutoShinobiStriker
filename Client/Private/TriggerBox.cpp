#include "TriggerBox.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Icon.h"

CTriggerBox::CTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CEventObject{ pDevice, pContext, eObjectID }
	, m_pGameManager { CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}

HRESULT CTriggerBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTriggerBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CTriggerBox::Priority_Update(_float fTimeDelta)
{
}

void CTriggerBox::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CTriggerBox::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameManager->Add_Object_ToCollision(TEXT("TriggerBox"), this, m_pColliderCom);
}

HRESULT CTriggerBox::Render()
{
	__super::Render();

	return S_OK;
}

void CTriggerBox::OnCollision()
{
	/* 자신의 트리거 타입 넘겨주기. */
	m_pGameManager->OnTrigger(m_eTriggerType);

	m_pColliderCom->Set_Active(false);
	m_IsDead = true;
	m_pIcon->Set_Dead(true);
}

HRESULT CTriggerBox::Ready_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC ColliderDesc{};

	ColliderDesc.fRadius = m_fColliderRadius;
	ColliderDesc.vCenter = m_vColliderCenter;
	ColliderDesc.isActive = true;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CTriggerBox* CTriggerBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CTriggerBox* pInstance = new CTriggerBox(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : Monster Spawner");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTriggerBox::Clone(void* pArg)
{
	CTriggerBox* pInstance = new CTriggerBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : Monster Spawner");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTriggerBox::Free()
{
	__super::Free();

	Safe_Release(m_pGameManager);
}
