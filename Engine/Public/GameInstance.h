#pragma once

#include "Prototype_Manager.h"

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

	_float Random_Normal();
	_float Random(_float fMin, _float fMax);

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
#pragma endregion

#pragma region PROTOTYPE_MANAGER
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CBase* pPrototype);
	class CBase* Clone_Prototype(PROTOTYPE ePrototype, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion

#pragma region OBJECT_MANAGER
	CComponent* Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	HRESULT Add_GameObject_ToLayer(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);
#pragma endregion


#pragma region RENDERER
	HRESULT Add_RenderGroup(RENDER eRenderGroup, class CGameObject* pRenderObject);
#pragma endregion





private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CPrototype_Manager*		m_pPrototype_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CPicking*					m_pPicking = { nullptr };

public:
	void Release_Engine();
	virtual void Free() override;
};

NS_END