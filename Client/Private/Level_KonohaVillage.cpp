#include "Level_KonohaVillage.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "MainCamera.h"
#include "TestCamera.h"
#include "ActionCamera.h"
#include "SkillActionCamera.h"
#include "NPCTalkCamera.h"

#include "TimerPanel.h"
#include "SkillSlotPanel.h"
#include "AttackTypePanel.h"
#include "ComboKOPanel.h"
#include "DialogUI.h"

CLevel_KonohaVillage::CLevel_KonohaVillage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
	, m_pGameManager{ CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}

HRESULT CLevel_KonohaVillage::Initialize()
{
	return E_NOTIMPL;
}

void CLevel_KonohaVillage::Update(_float fTimeDelta)
{
}

HRESULT CLevel_KonohaVillage::Render()
{
	return E_NOTIMPL;
}

HRESULT CLevel_KonohaVillage::Ready_Lights()
{
	return E_NOTIMPL;
}

HRESULT CLevel_KonohaVillage::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CLevel_KonohaVillage::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CLevel_KonohaVillage::Ready_Layer_Player(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CLevel_KonohaVillage::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CLevel_KonohaVillage::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CLevel_KonohaVillage::Ready_Layer_UI(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CLevel_KonohaVillage::Ready_Layer_StaticObjects(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CLevel_KonohaVillage::Ready_Layer_NPC(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CLevel_KonohaVillage::Ready_Layer_Map(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

CLevel_KonohaVillage* CLevel_KonohaVillage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	return nullptr;
}

void CLevel_KonohaVillage::Free()
{
}
