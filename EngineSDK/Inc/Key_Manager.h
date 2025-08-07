#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CKey_Manager final : public CBase
{
private:
	CKey_Manager();
	virtual ~CKey_Manager() = default;
	
public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Update();

public:
	_bool Key_Pressing(_ubyte byKey);
	_bool Key_Down(_ubyte byKey);
	_bool Key_Up(_ubyte byKey);

private:
	LPDIRECTINPUT8			m_pInputSDK = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = { nullptr };

private:
	_byte					m_byKeyState[KEY_MAX] = {};	
	_byte					m_byPreKeyState[KEY_MAX] = {};

public:
	static CKey_Manager* Create(HINSTANCE hInstance, HWND hWnd);
	virtual void Free() override;

};

NS_END


