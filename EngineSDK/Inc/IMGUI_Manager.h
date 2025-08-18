#pragma once

#include "Base.h"

NS_BEGIN(Engine)

/*
구현할 기능
1. 프로토타입 매니저 멤버 순회->
2. 오브젝트 매니저 멤버 순회->
3. 풀링 매니저 멤버 순회 ->
====================================================
4. 애니메이션, 본 정보 뽑아서 순회 가능하게끔 하기
*/

class ENGINE_DLL CIMGUI_Manager final : public CBase
{
private:
	CIMGUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CIMGUI_Manager() = default;

public:
	HRESULT Initialize(HWND hWnd, 
		class CPrototype_Manager* pPrototype_Manager, 
		class CObject_Manager* pObject_Manager, 
		class CPooling_Manager* pPooling_Manager,
		class CPicking *pPicking);
	void	Update(_float fTimeDelta);
	void	Render();
	void	Set_Visible_IMGUI(_bool bFlag, _uint iIMGUIID);
	void	Set_Visible_All_IMGUI(_bool bFlag);

public:
	void Clear();
private:
	HRESULT Ready_IMGUI(HWND hWnd);
	void	Release_IMGUI();

private:
	//여기서 IMGUI와 관련된 기능들을 추가해줄 것.
#pragma region MANAGERS
	void Show_Managers(_float fTimeDelta);
	void Show_PoolManager(_float fTimeDelta, _uint iLevelID);
	void Show_PrototypeManager(_float fTimeDelta, _uint iLevelID);
	void Show_ObjectManager(_float fTimeDelta, _uint iLevelID);
#pragma endregion 

#pragma region GAMEINFO
	//FPS, 현재 레벨 표시해주는 메서드
	void Show_GameInfo(_float fTimeDelta);
#pragma endregion

#pragma region PICKINGINFO
	//클라이언트에선 Edit 레벨에서만 쓸 수 있게 할 것.
	void Show_PickingInspector();
#pragma endregion

#pragma region INSPECTOR
	//Transform 등의 컴포넌트를 보여주는 메서드
	void Show_ObjectInspector(_float fTimeDelta);
#pragma endregion
private:
	_bool m_bVisibleFlag[ENUM_CLASS(IMGUI_VISIBLE::END)];

#pragma region MANAGERS
	_string strSelectedLayer = {};
	_string strSelectedPrototype = {};
#pragma endregion 

#pragma region GAMEINFO
	_uint	m_iFrameArraySize = {};
	_float* m_fFrames = { nullptr };
	_double m_fFrameRange[2] = { 50.f, 65.f };
	class CPrototype_Manager* m_pPrototype_Manager = { nullptr };
	class CObject_Manager* m_pObject_Manager = { nullptr };
	class CPooling_Manager* m_pPooling_Manager = { nullptr };
	class CPicking* m_pPicking = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
#pragma endregion


#pragma region INSPECTOR
	class CGameObject* m_pSelectedGameObject = { nullptr };
	_string m_strSelectedLayer = {};
	_uint m_iSelectedObjectID = {};
	_uint m_iSelectedObjectIndex = {};

	_float3 m_vObjectPos = {};
	_float3 m_vObjectScale = {};
	_float m_fAngleRight = { 0.f }, m_fAngleUp = { 0.f }, m_fAngleLook = { 0.f };

	_float3 m_vTargetPos = {};
	_float	m_fLimitDistance = {};
#pragma endregion

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static CIMGUI_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, 
		class CPrototype_Manager* pPrototype_Manager, 
		class CObject_Manager* pObject_Manager,
		class CPooling_Manager* pPooling_Manager,
		class CPicking* pPicking);

	virtual void Free() override;
};

NS_END
