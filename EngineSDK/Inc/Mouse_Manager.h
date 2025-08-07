#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CMouse_Manager final : public CBase
{
private:
	CMouse_Manager();
	virtual ~CMouse_Manager() = default;

public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Update();

public:
	_bool	Mouse_Down(MOUSEKEYSTATE eMouse);
	_bool	Mouse_Up(MOUSEKEYSTATE eMouse);
	_long	Get_MouseMove(MOUSEMOVESTATE eMouseState) {
		return *((reinterpret_cast<_long*>(&m_tMouseState)) + ENUM_CLASS(eMouseState));
	}
private:
	LPDIRECTINPUT8			m_pInputSDK = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pMouse = { nullptr };

private:
	DIMOUSESTATE			m_tPreMouseState = {};
	DIMOUSESTATE			m_tMouseState = {};

public:
	static CMouse_Manager* Create(HINSTANCE hInstance, HWND hWnd);
	virtual void Free() override;
};

NS_END
