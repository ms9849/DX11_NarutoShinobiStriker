#include "TriggerBox.h"

#include "GameInstance.h"

CTriggerBox::CTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CTriggerBox::CTriggerBox(const CTriggerBox& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CTriggerBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTriggerBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	TRIGGER_BOX_DESC* pDesc = static_cast<TRIGGER_BOX_DESC*>(pArg);
	
	m_vColliderCenter = pDesc->vCenter;
	m_fColliderRadius = pDesc->fRadius;

	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&pDesc->vPosition));

	return S_OK;
}

void CTriggerBox::Priority_Update(_float fTimeDelta)
{
}

void CTriggerBox::Update(_float fTimeDelta)
{
	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CTriggerBox::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CTriggerBox::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CTriggerBox::Free()
{
	__super::Free();
	
	Safe_Release(m_pColliderCom);
}
