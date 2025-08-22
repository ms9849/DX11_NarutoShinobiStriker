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
		class CPicking_Manager* pPicking);
	void	Update(_float fTimeDelta);
	void	Render();
	void	Set_Visible_IMGUI(_bool bFlag, _uint iIMGUIID);
	void	Set_Visible_All_IMGUI(_bool bFlag);
	void    Align_Center(const _char* pText);

public:
	void Clear();
private:
	HRESULT Ready_IMGUI(HWND hWnd);
	void	Release_IMGUI();

private:
	//여기서 IMGUI와 관련된 기능들을 추가해줄 것.
#pragma region GAMEINFO
	//FPS 및 여러 정보들 표시해주는 메서드
	void Show_GameInfo(_float fTimeDelta);
#pragma endregion

#pragma region INSPECTOR
	//Transform 등의 컴포넌트를 보여주는 메서드
	void Show_ObjectInspector(_float fTimeDelta);
#pragma endregion

#pragma region EDITOR
	void Show_Editor(_float fTimeDelta);
	void Map_Editor();
	void Animation_Editor();
	void Effect_Editor();
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
	class CPicking_Manager* m_pPicking_Manager = { nullptr };
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


#pragma region EDITOR
	_char m_szClonePrototype[256] = {};
	_int m_iClonePrototypeLevel = {};
	_char m_szLayerTag[256] = {};
	_int m_iLayerLevel = {};

#pragma endregion
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static CIMGUI_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, 
		class CPrototype_Manager* pPrototype_Manager, 
		class CObject_Manager* pObject_Manager,
		class CPooling_Manager* pPooling_Manager,
		class CPicking_Manager* pPicking);

	virtual void Free() override;
};

NS_END
