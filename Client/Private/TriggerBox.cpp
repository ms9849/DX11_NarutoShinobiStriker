#include "TriggerBox.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Icon.h"
#include "EffectContainer.h"
#include "EffectObject.h"

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

	/* 미션 박스용 이펙트 */
	CEffectContainer::EFFECT_CONTAINER_DESC EffectDesc;
	EffectDesc.IsBinary = true;
	EffectDesc.strFilePath = TEXT("../Bin/Resources/Effects/Mission_eff.bin");
	
	m_pEffectMain = static_cast<CEffectContainer*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_EffectContainer"),
		&EffectDesc));

	m_pEffectMain->Set_Blur(true);

	Safe_AddRef(m_pEffectMain);
	m_pGameInstance->Add_Clone_ToLayer(m_pEffectMain, m_pGameInstance->Get_LevelID(), TEXT("Layer_Effect"));

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
	m_pEffectMain->Set_ParentMatrix(XMMatrixScaling(5.f, 3.f, 5.f) * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
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
	m_pEffectMain->Set_Visible(false);

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
	Safe_Release(m_pEffectMain);
}
