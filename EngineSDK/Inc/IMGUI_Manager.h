#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CIMGUI_Manager final : public CBase
{
private:
	CIMGUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CIMGUI_Manager() = default;

public:
	HRESULT Initialize(HWND hWnd, class CPrototype_Manager* pPrototype_Manager, class CObject_Manager* pObject_Manager);
	void	Update_IMGUI();
	void	Render_IMGUI();

private:
	HRESULT Ready_IMGUI(HWND hWnd);
	void	Release_IMGUI();

private:
	//여기서 IMGUI와 관련된 기능들을 추가해줄 것.

private:
	_int m_iBufferSize = { 256 };
	_char* m_pCharBuffer = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CPrototype_Manager* m_pPrototype_Manager = { nullptr };
	class CObject_Manager* m_pObject_Manager = { nullptr };

public:
	static CIMGUI_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, 
		class CPrototype_Manager* pPrototype_Manager, 
		class CObject_Manager* pObject_Manager);

	virtual void Free() override;
};

NS_END
