#include "MonsterSpawner.h"

#include "GameInstance.h"
#include "GameManager.h"

CMonsterSpawner::CMonsterSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CTriggerBox{ pDevice, pContext, eObjectID }
	, m_pGameManager { CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}

HRESULT CMonsterSpawner::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonsterSpawner::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CMonsterSpawner::Priority_Update(_float fTimeDelta)
{
}

void CMonsterSpawner::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CMonsterSpawner::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameManager->Add_Object_ToCollision(TEXT("TriggerBox"), this, m_pColliderCom);
}

HRESULT CMonsterSpawner::Render()
{
	__super::Render();

	return S_OK;
}

void CMonsterSpawner::OnCollision()
{
	/* 자신의 트리거 타입 넘겨주기. */
	m_pGameManager->OnTrigger(m_eTriggerType);

	m_pColliderCom->Set_Active(false);
	m_IsDead = true;
}

HRESULT CMonsterSpawner::Ready_Components()
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

CMonsterSpawner* CMonsterSpawner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CMonsterSpawner* pInstance = new CMonsterSpawner(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : Monster Spawner");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonsterSpawner::Clone(void* pArg)
{
	CMonsterSpawner* pInstance = new CMonsterSpawner(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : Monster Spawner");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterSpawner::Free()
{
	__super::Free();

	Safe_Release(m_pGameManager);
}
