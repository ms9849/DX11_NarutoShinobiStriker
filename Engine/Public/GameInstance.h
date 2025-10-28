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
	/* 리턴 값은 라디안 */
	_float   Calc_Dot(_fvector vVector1, _fvector vVector2);
	/* 일차함수 계산기 */
	_float   Calc_Linear(_float fLinear, _float fConstant, _float fValue);
	/* 이차함수 계산기 */
	_float   Calc_Quadratic(_float fQuad, _float fLinear, _float fConstant, _float fValue);
	_float	 Random_Normal();
	_float	 Random(_float fMin, _float fMax);
	/* Clamp 기능. 직접 구현해서 사용. */
	_float	 Clamp_Float(_float fValue, _float fMin, _float fMax);

	_string	 ToString(_wstring wStr);
	_wstring ToWstring(_string Str);
	_float	 Lerp_Float(_float fSource, _float fDest, _float fLerpRate);
	_vector  Clamp_Position_ToViewPort(_fvector vPosition);
	_bool	 IsInViewPort(_fvector vPosition, _float fRatioX = 1.f, _float fRatioY = 1.f, _float* fPosX = nullptr, _float* fPosY = nullptr);
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
	void	Set_Distortion(_float fTime);
	void	Set_RadialBlur(_float fTime);
	HRESULT Add_RenderGroup(RENDER eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Add_Font(class CFont* pRenderFont);
	/* 맵을 위한 스태틱 섀도우 함수. 부를때마다 초기화되니까 조심할 것.*/
	void	Add_StaticShadow(class CGameObject* pGameObject);
#ifdef _DEBUG
	HRESULT Add_DebugComponent(class CComponent* pDebugComponent);
#endif

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
	_float3 Get_RayPos(RAY eRayType);
	_float3 Get_RayDir(RAY eRayType);
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

#pragma region LIGHT_MANAGER
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer* pVIBuffer);
	void Clear_Lights();

	const LIGHT_DESC* Get_Desc(_int iIdx);
	_int Get_LightSize();
	void Pop_Light();
#pragma endregion

#pragma region Physx_Manager
	void	Add_GameObject_ToPhysx(class CGameObject* pGameObject);
	void	Add_Geometry_ToPhysx(class CGameObject* pGameObject, class CModel* pModel);

	void	Clear_Geometry();
	void	Calc_Geometry();
	void	Check_GeometryCollision();
	_bool	Check_GameObject_GeometryCollision(class CGameObject* pGameObject, _bool* IsCollision = nullptr);
	_bool	Check_GeometryPicking();
	_bool	Check_Ray_GeometryPicking(_float3 vRayPos, _float3 vRayDir, _float3* vResultPos, _float* fResultDist);
#pragma endregion


#pragma region Target_Manager
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT End_MRT();
	HRESULT Bind_RenderTarget(const _wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);
#ifdef _DEBUG
	HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_RT_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer);
#endif
#pragma endregion

#pragma region SHADOW 
	HRESULT Ready_Shadow_Light(const SHADOW_LIGHT_DESC& Desc);
	HRESULT Bind_Shadow_Resource(class CShader* pShader, const _char* pConstantName, D3DTS eType);
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
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CPhysx_Manager*			m_pPhysxManager = { nullptr };
	class CTarget_Manager*			m_pTarget_Manager = { nullptr };
	class CShadow*					m_pShadow = { nullptr };

private:
	_uint m_iWinSizeX = {};
	_uint m_iWinSizeY = {};

public:
	void Release_Engine();
	virtual void Free() override;
};

NS_END