#include "GameInstance.h"

#include "Pooling_Manager.h"
#include "Prototype_Manager.h"
#include "Object_Manager.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Timer_Manager.h"
#include "Sound_Manager.h"
#include "Key_Manager.h"
#include "Renderer.h"
#include "IMGUI_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWindowMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pPrototype_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;
	
	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pSound_Manager = CSound_Manager::Create();
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	m_pKey_Manager = CKey_Manager::Create();
	if (nullptr == m_pKey_Manager)
		return E_FAIL;

	m_pPooling_Manager = CPooling_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pPooling_Manager)
		return E_FAIL;

	m_pIMGUI_Manager = CIMGUI_Manager::Create(*ppDevice, *ppContext, EngineDesc.hWnd, m_pPrototype_Manager, m_pObject_Manager, m_pPooling_Manager);
	if (nullptr == m_pIMGUI_Manager)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	/* 객체 업데이트 계층 */
	m_pObject_Manager->Priority_Update(fTimeDelta);

	m_pObject_Manager->Update(fTimeDelta);

	m_pObject_Manager->Late_Update(fTimeDelta);

	m_pObject_Manager->Clear_DeadObj();

	/* 풀링 매니저 업데이트 */
	m_pPooling_Manager->Update();

	/* 레벨 업데이트 */
	m_pLevel_Manager->Update(fTimeDelta);
	
	/* IMGUI 업데이트 */
	m_pIMGUI_Manager->Update(fTimeDelta);

	/* 키 매니저 초기화 */
	m_pKey_Manager->Update();
}

HRESULT CGameInstance::Draw()
{
	m_pRenderer->Render();

	m_pLevel_Manager->Render();

	/* IMGUI 렌더 */
	m_pIMGUI_Manager->Render();

	return S_OK;
}

void CGameInstance::Clear_Resources(_uint iLevelIndex)
{
	m_pPrototype_Manager->Clear(iLevelIndex);
	m_pObject_Manager->Clear(iLevelIndex);
	m_pPooling_Manager->Clear(iLevelIndex);
}

#pragma region TOOLS
_float CGameInstance::Random_Normal()
{
	return static_cast<_float>(rand()) / RAND_MAX;	
}

_float CGameInstance::Random(_float fMin, _float fMax)
{
	return fMin + Random_Normal() * (fMax - fMin);	
}

_string CGameInstance::ToString(_wstring wStr)
{
	_uint iSize = WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(),(_uint)wStr.size(), nullptr, 0, nullptr, nullptr);

	_string strTo(iSize, 0);
	WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(),(_uint)wStr.size(), &strTo[0], iSize, nullptr, nullptr);

	return strTo;
}

_wstring CGameInstance::ToWstring(_string Str)
{
	_uint iSize = MultiByteToWideChar(CP_UTF8, 0, Str.c_str(), (_uint)Str.size(), nullptr, 0);

	_wstring Result(iSize, 0);
	MultiByteToWideChar(CP_UTF8, 0, Str.c_str(),(_int)Str.size(), &Result[0], iSize);

	return Result;
}

#pragma endregion

#pragma region GRAPHIC_DEVICE

void CGameInstance::Render_Begin(const _float4* pClearColor)
{
	m_pGraphic_Device->Clear_BackBuffer_View(pClearColor);
	m_pGraphic_Device->Clear_DepthStencil_View();
}

void CGameInstance::Render_End()
{
	m_pGraphic_Device->Present();
}

#pragma endregion

#pragma region TIMER_MANAGER

_float CGameInstance::Get_TimeDelta(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

void CGameInstance::Compute_TimeDelta(const _wstring& strTimerTag)
{
	m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

#pragma endregion

#pragma region LEVEL_MANAGER

HRESULT CGameInstance::Change_Level(CLevel* pNewLevel)
{
	return m_pLevel_Manager->Change_Level(pNewLevel);
}

_uint CGameInstance::Get_LevelID()
{
	return m_pLevel_Manager->Get_LevelID();
}

#pragma endregion

#pragma region PROTOTYPE_MANAGER

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	return m_pPrototype_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CBase* CGameInstance::Clone_Prototype(PROTOTYPE ePrototype, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pPrototype_Manager->Clone_Prototype(ePrototype, iLevelIndex, strPrototypeTag, pArg);;
}

#pragma endregion

#pragma region OBJECT_MANAGER

CComponent* CGameInstance::Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);	
}

HRESULT CGameInstance::Add_GameObject_ToLayer(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	return m_pObject_Manager->Add_GameObject_ToLayer(iPrototypeLevelIndex, strPrototypeTag, iLayerLevelIndex, strLayerTag, pArg);
}

HRESULT CGameInstance::Add_Clone_ToLayer(CGameObject* pClone, _uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Add_Clone_ToLayer(pClone, iLayerLevelIndex, strLayerTag);
}

#pragma endregion

#pragma region POOLING_MANAGER
void CGameInstance::Add_GameObject_ToPool(_uint iLevelIndex, CGameObject* pGameObject)
{
	m_pPooling_Manager->Add_GameObject_ToPool(iLevelIndex, pGameObject);
}
HRESULT CGameInstance::Add_PoolingObject_ToLayer(const _wstring& strPoolingTag, _uint iPoolingOjbectLevelIndex, _uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	return m_pPooling_Manager->Add_PoolingObject_ToLayer(strPoolingTag, iPoolingOjbectLevelIndex, iLayerLevelIndex, strLayerTag);
}
#pragma endregion

#pragma region RENDERER

HRESULT CGameInstance::Add_RenderGroup(RENDER eRenderGroup, CGameObject* pRenderObject)
{
	return m_pRenderer->Add_RenderGroup(eRenderGroup, pRenderObject);
}

#pragma endregion

#pragma region SOUND_MANAGER

void CGameInstance::PlaySoundOnce(const _wstring& pSoundKey, CHANNELID eID, float fVolume)
{
	m_pSound_Manager->PlaySoundOnce(pSoundKey, eID, fVolume);
}

void CGameInstance::PlaySoundLoop(const _wstring& pSoundKey, CHANNELID eID, float fVolume)
{
	m_pSound_Manager->PlaySoundLoop(pSoundKey, eID, fVolume);
}

void CGameInstance::PauseBGM(_bool bFlag)
{
	m_pSound_Manager->PauseBGM(bFlag);
}

void CGameInstance::PlayBGM(const _wstring& pSoundKey, float fVolume)
{
	m_pSound_Manager->PlayBGM(pSoundKey, fVolume);
}

void CGameInstance::StopSound(CHANNELID eID)
{
	m_pSound_Manager->StopSound(eID);
}

void CGameInstance::StopAll()
{
	m_pSound_Manager->StopAll();
}

void CGameInstance::SetChannelVolume(CHANNELID eID, float fVolume)
{
	m_pSound_Manager->SetChannelVolume(eID, fVolume);
}

#pragma endregion

#pragma region KEY_MANAGER

void CGameInstance::Key_Input()
{
	m_pKey_Manager->Key_Input();
}

_bool CGameInstance::Key_Pressing(_uint _iKey)
{
	return m_pKey_Manager->Key_Pressing(_iKey);
}

_bool CGameInstance::Key_Up(_uint _iKey)
{
	return m_pKey_Manager->Key_Up(_iKey);
}

_bool CGameInstance::Key_Down(_uint _iKey)
{
	return m_pKey_Manager->Key_Down(_iKey);
}

#pragma endregion

#pragma region IMGUI_MANAGER

void CGameInstance::Set_Visible_IMGUI(_bool bFlag, _uint iIMGUIID)
{
	m_pIMGUI_Manager->Set_Visible_IMGUI(bFlag, iIMGUIID);
}

void CGameInstance::Set_Visible_All_IMGUI(_bool bFlag)
{
	m_pIMGUI_Manager->Set_Visible_All_IMGUI(bFlag);
}

#pragma endregion

void CGameInstance::Release_Engine()
{
	DestroyInstance();

	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pPrototype_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pPooling_Manager);
	Safe_Release(m_pIMGUI_Manager);
}

void CGameInstance::Free()
{
	__super::Free();
}
