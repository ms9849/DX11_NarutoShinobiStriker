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
	_float Lerp_Float(_float fSource, _float fDest, _float fLerpRate);
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
	CBase* Clone_Prototype(PROTOTYPE ePrototype, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
	CBase* Get_Prototype(_uint iLevelID, const _wstring& strPrototypeTag);
#pragma endregion

#pragma region OBJECT_MANAGER
	CGameObject* Get_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex = 0);
	CComponent* Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	size_t	Get_LayerSize(_uint iLevelIndex, const _wstring& strLayerTag);

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
	void Set_Pipeline_Matrix(D3DTS eState, _fmatrix PipeLineMatrix);

	const _float4x4* Get_PipeLine_Float4x4(D3DTS eState);
	_matrix Get_PipeLine_Matrix(D3DTS eState);

	const _float4x4* Get_PipeLine_InverseFloat4x4(D3DTS eState);
	_matrix Get_PipeLine_InverseMatrix(D3DTS eState);

	const _float4* Get_CamState(STATE eState);
#pragma endregion

#pragma region PICKING_MANAGER
	_bool	Picking(_uint iLevelIdx, _float3* pOut);
	HRESULT Add_GameObject_ToPicking(_uint iLevelIdx, class CGameObject* pGameObject, class CVIBuffer* pVIBuffer);
	HRESULT Add_GameObject_ToPicking(_uint iLevelIdx, class CGameObject* pGameObject, class CModel* pModel);
	_bool	Picking_InWorldSpace(_fvector vPointA, _fvector vPointB, _fvector vPointC, _float3* pOut);
	void	Transform_Picking_ToLocalSpace(_fmatrix WorldMatrixInverse);
	_bool	Picking_InLocalSpace(_fvector vPointA, _fvector vPointB, _fvector vPointC, _float3* pOut);
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

#pragma region INPUT_MANAGER
	_bool		Key_Pressing(_ubyte byKey);
	_bool		Key_Up(_ubyte byKey);
	_bool		Key_Down(_ubyte byKey);
	_bool		Mouse_Down(MOUSEKEYSTATE eMouse);
	_bool		Mouse_Up(MOUSEKEYSTATE eMouse);
	_bool		Mouse_Pressing(MOUSEKEYSTATE eMouse);
	_long		Get_MouseMove(MOUSEMOVESTATE eMouseState);
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
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CIMGUI_Manager*			m_pIMGUI_Manager = { nullptr };
	class CPicking_Manager*			m_pPicking_Manager = { nullptr };
	class CSound_Manager*			m_pSound_Manager = { nullptr };
	class CInput_Manager*			m_pInput_Manager = { nullptr };
	class CPooling_Manager*			m_pPooling_Manager = { nullptr };

public:
	void Release_Engine();
	virtual void Free() override;
};

NS_END