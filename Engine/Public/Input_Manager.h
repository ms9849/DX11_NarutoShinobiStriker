#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CInput_Manager final : public CBase
{
private:
	CInput_Manager();
	virtual ~CInput_Manager() = default;
	
public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Update();

public:
	_bool Key_Pressing(_ubyte byKey);
	_bool Key_Down(_ubyte byKey);
	_bool Key_Up(_ubyte byKey);

	_bool	Mouse_Down(MOUSEKEYSTATE eMouse);
	_bool	Mouse_Up(MOUSEKEYSTATE eMouse);
	_bool   Mouse_Pressing(MOUSEKEYSTATE eMouse);

	_long	Get_MouseMove(MOUSEMOVESTATE eMouseState) {
		return *((reinterpret_cast<_long*>(&m_tMouseState)) + ENUM_CLASS(eMouseState));
	}

private:
	LPDIRECTINPUT8			m_pInputSDK = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pMouse = { nullptr };

private:
	_byte					m_byKeyState[KEY_MAX] = {};	
	_byte					m_byPreKeyState[KEY_MAX] = {};

	DIMOUSESTATE			m_tPreMouseState = {};
	DIMOUSESTATE			m_tMouseState = {};

public:
	static CInput_Manager* Create(HINSTANCE hInstance, HWND hWnd);
	virtual void Free() override;

};

NS_END


