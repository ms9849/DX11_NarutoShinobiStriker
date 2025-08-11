#include "SkillSlotPanel.h"

#include "GameInstance.h"
#include "SkillSlot.h"
#include "GameManager.h"

CSkillSlotPanel::CSkillSlotPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CPanel { pDevice, pContext, eObjectID }
{
}

CSkillSlotPanel::CSkillSlotPanel(const CSkillSlotPanel& rhs)
	: CPanel { rhs }
{
}

HRESULT CSkillSlotPanel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkillSlotPanel::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_SkillSlots()))
		return E_FAIL;

	return S_OK;
}

void CSkillSlotPanel::Priority_Update(_float fTimeDelta)
{
}

void CSkillSlotPanel::Update(_float fTimeDelta)
{
}

void CSkillSlotPanel::Late_Update(_float fTimeDelta)
{
}

HRESULT CSkillSlotPanel::Render()
{
	return S_OK;
}

HRESULT CSkillSlotPanel::Ready_SkillSlots()
{
	/* 여기서 플레이어 정보랑 묶어주면 된다. 몇번째 스킬 가져올건지 등.. */
	for (_int i = 0; i < 3; ++i)
	{
		CUIObject::UIOBJECT_DESC Desc;

		switch (i)
		{
		case 0:
			Desc = CUIObject::CreateDesc(m_fX - 100, m_fY, m_fZ - 0.05f, 94.f, 94.f, 0.f);
			break;

		case 1:
			Desc = CUIObject::CreateDesc(m_fX, m_fY, m_fZ - 0.05f, 94.f, 94.f, 0.f);
			break;

		case 2:
			Desc = CUIObject::CreateDesc(m_fX + 100, m_fY, m_fZ - 0.05f, 94.f, 94.f, 0.f);
			break;

		default:
			break;
		}

		CSkillSlot* pDecimal = static_cast<CSkillSlot*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Prototype_GameObject_SkillSlotUI"), &Desc));
		if (nullptr == pDecimal)
			return E_FAIL;

		m_pGameInstance->Add_Clone_ToLayer(pDecimal, ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Layer_UI"));
		m_Childs.push_back(pDecimal);
		Safe_AddRef(pDecimal);
	}

	return S_OK;
}

CSkillSlotPanel* CSkillSlotPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CSkillSlotPanel* pInstance = new CSkillSlotPanel(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : SkillSlotPanel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkillSlotPanel::Clone(void* pArg)
{
	CSkillSlotPanel* pInstance = new CSkillSlotPanel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : SkillSlotPanel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkillSlotPanel::Free()
{
	__super::Free();
}
