#include "EventObject.h"

#include "GameInstance.h"

#include "Icon.h"

CEventObject::CEventObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CEventObject::CEventObject(const CEventObject& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CEventObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEventObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_MissionIcon()))
		return E_FAIL;

	EVENT_COLLIDER_DESC* pDesc = static_cast<EVENT_COLLIDER_DESC*>(pArg);
	
	m_vColliderCenter = pDesc->vCenter;
	m_fColliderRadius = pDesc->fRadius;
	m_eTriggerType = pDesc->eTriggerType;
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));

	return S_OK;
}

void CEventObject::Priority_Update(_float fTimeDelta)
{
}

void CEventObject::Update(_float fTimeDelta)
{
	m_pIcon->Set_Position(m_pTransformCom->Get_State(STATE::POSITION) + XMVectorSet(0.f, 1.5f, 0.f, 1.f));
	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CEventObject::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CEventObject::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

HRESULT CEventObject::Ready_MissionIcon()
{
	CIcon::ICON_DESC Desc;
	Desc.iTextureIdx = 1;

	m_pIcon = static_cast<CIcon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_pGameInstance->Get_LevelID(),
		TEXT("Prototype_GameObject_Icon"), &Desc));

	Safe_AddRef(m_pIcon);
	m_pGameInstance->Add_Clone_ToLayer(m_pIcon, m_pGameInstance->Get_LevelID(), TEXT("Layer_UI"));

	return S_OK;
}

void CEventObject::Free()
{
	__super::Free();
	
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pIcon);
}
