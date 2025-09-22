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

	m_eSpawnerType = static_cast<MONSTER_SPAWNER_DESC*>(pArg)->eSpawnerType;

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
	/* 여기서 제츠 소환 로직. */
	if (SPAWNER_TYPE::TUTORIAL_01 == m_eSpawnerType)
	{
		m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_WhiteJetsu"),
			ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_Monster"));
	}

	m_isDead = true;
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
