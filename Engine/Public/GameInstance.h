#pragma once

#include "Prototype_Manager.h"
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance);

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);
	HRESULT Draw();
	void Clear_Resources(_uint iLevelIndex);

#pragma region TOOLS
	_float	Random_Normal();
	_float	Random(_float fMin, _float fMax);

	_string	ToString(_wstring wStr);
	_wstring ToWstring(_string Str);

	/* 현재 마우스의 위치를 반환 */
	POINT	Get_MousePos();
	/* 이전 프레임과 현재 프레임 간의 마우스 좌표 차이를 반환*/
	POINT	Get_MouseDiff();
	void	Calc_MousePos();
#pragma endregion

#pragma region GRAPHIC_DEVICE
public:
	void Render_Begin(const _float4* pClearColor);
	void Render_End();
	
#pragma endregion

#pragma region TIMER_MANAGER
	_float	Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	void	Compute_TimeDelta(const _wstring& strTimerTag);
#pragma endregion

#pragma region LEVEL_MANAGER
public:
	HRESULT Change_Level(class CLevel* pNewLevel);
	_uint	Get_LevelID();
	_bool	IsLevelChangeRequested();
	void	Request_LevelChange();
#pragma endregion

#pragma region PROTOTYPE_MANAGER
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CBase* pPrototype);
	class CBase* Clone_Prototype(PROTOTYPE ePrototype, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion

#pragma region OBJECT_MANAGER
	CComponent* Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	HRESULT Add_GameObject_ToLayer(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);
	HRESULT Add_Clone_ToLayer(class CGameObject* pClone, _uint iLayerLevelIndex, const _wstring& strLayerTag);
#pragma endregion

#pragma region POOLING_MANAGER
	void	Add_GameObject_ToPool(_uint iLevelIndex, class CGameObject* pGameObject);
	HRESULT	Add_PoolingObject_ToLayer(const _wstring& strPoolingTag, _uint iPoolingOjbectLevelIndex, CGameObject::GAMEOBJECT_DESC* pDesc,_uint iLayerLevelIndex, const _wstring& strLayerTag);
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderGroup(RENDER eRenderGroup, class CGameObject* pRenderObject);
#pragma endregion

#pragma region PIPELINE
	const _float4x4& Get_ViewMatrix();
	const _float4x4& Get_CameraWorldMatrix();
	const _float4x4& Get_ProjMatrix();
	void Set_CameraWorldMatrix(const _float4x4& CameraWorldMatrix);
	void Set_ProjMatrix(const _float4x4& ProjMatrix);
#pragma endregion

#pragma region SOUND_MANAGER
	void PlaySoundOnce(const _wstring& pSoundKey, CHANNELID eID, float fVolume);
	void PlaySoundLoop(const _wstring& pSoundKey, CHANNELID eID, float fVolume);
	void PauseBGM(_bool bFlag);
	void PlayBGM(const _wstring& pSoundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);
#pragma endregion

#pragma region KEY_MANAGER
	void		Key_Input();
	_bool		Key_Pressing(_uint _iKey);
	_bool		Key_Up(_uint _iKey);
	_bool		Key_Down(_uint _iKey);
#pragma endregion

#pragma region IMGUI_MANAGER
	void	Set_Visible_IMGUI(_bool bFlag, _uint iIMGUIID);
	void	Set_Visible_All_IMGUI(_bool bFlag);
#pragma endregion

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CPrototype_Manager*		m_pPrototype_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CPicking*					m_pPicking = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CSound_Manager*			m_pSound_Manager = { nullptr };
	class CKey_Manager*				m_pKey_Manager = { nullptr };
	class CPooling_Manager*			m_pPooling_Manager = { nullptr };
	class CIMGUI_Manager*			m_pIMGUI_Manager = { nullptr };

	/*클라이언트의 윈도우 핸들*/
	HWND							m_hWnd;
	POINT							m_ptMousePos{}, m_ptPreMousePos{};
public:
	void Release_Engine();
	virtual void Free() override;
};

NS_END