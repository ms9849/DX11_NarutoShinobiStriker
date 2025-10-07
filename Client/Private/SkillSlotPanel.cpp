#include "SkillSlotPanel.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Player.h"

#include "SkillSlotUI.h"
#include "ProgressBarUI.h"


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

	if (FAILED(Ready_ProgressBar()))
		return E_FAIL;

	Set_MaxHpProgress(m_pGameManager->Get_PlayerPtr()->Get_MaxHp());

	return S_OK;
}

void CSkillSlotPanel::Priority_Update(_float fTimeDelta)
{
}

void CSkillSlotPanel::Update(_float fTimeDelta)
{
	Set_HpProgress(m_pGameManager->Get_PlayerPtr()->Get_CurrentHp());
}

void CSkillSlotPanel::Late_Update(_float fTimeDelta)
{
}

HRESULT CSkillSlotPanel::Render()
{
	return S_OK;
}

void CSkillSlotPanel::Set_SkillSlot_Visible(_bool bFlag)
{
	for (_uint i = 0; i < 4; ++i)
	{
		static_cast<CSkillSlotUI*>(m_Childs[i])->Set_Visible(bFlag);
	}

	static_cast<CProgressBarUI*>(m_Childs[4])->Set_Visible(bFlag);
}

void CSkillSlotPanel::Set_HpProgress(_float fProgress)
{
	static_cast<CProgressBarUI*>(m_Childs[4])->Set_Progress(fProgress);
}

void CSkillSlotPanel::Set_MaxHpProgress(_float fMaxProgress)
{
	static_cast<CProgressBarUI*>(m_Childs[4])->Set_MaxProgress(fMaxProgress);
}

void CSkillSlotPanel::Change_Skill(_uint iSkillIdx, SKILL eSkill)
{
	static_cast<CSkillSlotUI*>(m_Childs[iSkillIdx])->Change_Skill(eSkill);
}

HRESULT CSkillSlotPanel::Ready_SkillSlots()
{
	CSkillSlotUI::SKILLSLOT_DESC SrcDesc;
	UIOBJECT_DESC DstDesc;

	/* 여기서 플레이어 정보랑 묶어주면 된다. 몇번째 스킬 가져올건지 등.. */
	for (_int i = 0; i < 4; ++i)
	{
		switch (i)
		{
		case 0:
			DstDesc = CUIObject::CreateDesc(m_fX - 100, m_fY, m_fZ - 0.05f, 72.f, 66.f, 0, 0.f);
			memcpy(&SrcDesc, &DstDesc, sizeof(UIOBJECT_DESC));
			SrcDesc.iSkillNum = ENUM_CLASS(SKILLNUM::FIRST);
			break;

		case 1:
			DstDesc = CUIObject::CreateDesc(m_fX, m_fY, m_fZ - 0.05f, 80.f, 76.f, 0, 0.f);
			memcpy(&SrcDesc, &DstDesc, sizeof(UIOBJECT_DESC));
			SrcDesc.iSkillNum = ENUM_CLASS(SKILLNUM::SECOND);
			break;

		case 2:
			DstDesc = CUIObject::CreateDesc(m_fX + 100, m_fY, m_fZ - 0.05f, 80.f, 76.f, 0, 0.f);
			memcpy(&SrcDesc, &DstDesc, sizeof(UIOBJECT_DESC));
			SrcDesc.iSkillNum = ENUM_CLASS(SKILLNUM::THIRD);
			break;

		case 3:
			DstDesc = CUIObject::CreateDesc(m_fX - 875, m_fY, m_fZ - 0.05f, 400.f, 100.f, 0, 0.f);
			memcpy(&SrcDesc, &DstDesc, sizeof(UIOBJECT_DESC));
			SrcDesc.iSkillNum = ENUM_CLASS(SKILLNUM::SPECIAL);
			break;


		default:
			break;
		}

		CSkillSlotUI* pSkillSlot = static_cast<CSkillSlotUI*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Prototype_GameObject_SkillSlotUI"), &SrcDesc));
		if (nullptr == pSkillSlot)
			return E_FAIL;

		m_pGameInstance->Add_Clone_ToLayer(pSkillSlot, ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Layer_UI"));
		m_Childs.push_back(pSkillSlot);
		Safe_AddRef(pSkillSlot);
	}

	return S_OK;
}

HRESULT CSkillSlotPanel::Ready_ProgressBar()
{
	UIOBJECT_DESC Desc;
	Desc = CUIObject::CreateDesc(m_fX - 825, m_fY + 19.25, m_fZ - 0.10f, 230.f, 46.f, 2, 0.f);

	//여기서 Progress바 하나 생성.
	CProgressBarUI* pProgressBar = static_cast<CProgressBarUI*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ProgressBarUI"), &Desc));
	m_pGameInstance->Add_Clone_ToLayer(pProgressBar, ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Layer_UI"));
	m_Childs.push_back(pProgressBar);
	Safe_AddRef(pProgressBar);

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
{	CSkillSlotPanel* pInstance = new CSkillSlotPanel(*this);

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
