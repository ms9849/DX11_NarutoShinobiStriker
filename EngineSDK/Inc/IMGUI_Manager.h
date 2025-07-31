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
		class CPooling_Manager* pPooling_Manager);
	void	Update(_float fTimeDelta);
	void	Render();
	void	Set_Visible_IMGUI(_bool bFlag, _uint iIMGUIID);
	void	Set_Visible_All_IMGUI(_bool bFlag);

private:
	HRESULT Ready_IMGUI(HWND hWnd);
	void	Release_IMGUI();

private:
	//여기서 IMGUI와 관련된 기능들을 추가해줄 것.
	void Show_Managers();
	void Show_PoolManager(_uint iLevelID);
	void Show_PrototypeManager(_uint iLevelID);
	void Show_ObjectManager(_uint iLevelID);

	void Show_GameInfo(_float fTimeDelta);
private:
	_bool m_bVisibleFlag[ENUM_CLASS(IMGUI_VISIBLE::END)];
	_string strSelectedLayer = {};
	_string strSelectedPrototype = {};
	_uint	m_iFrameArraySize = {};
	_float* m_fFrames = {};
	_double m_fFrameRange[2] = { 50.f, 65.f };

	class CPrototype_Manager* m_pPrototype_Manager = { nullptr };
	class CObject_Manager* m_pObject_Manager = { nullptr };
	class CPooling_Manager* m_pPooling_Manager = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static CIMGUI_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, 
		class CPrototype_Manager* pPrototype_Manager, 
		class CObject_Manager* pObject_Manager,
		class CPooling_Manager* pPooling_Manager);

	virtual void Free() override;
};

NS_END
