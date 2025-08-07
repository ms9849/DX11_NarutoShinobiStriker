#include "Mouse_Manager.h"

CMouse_Manager::CMouse_Manager()
{
}

HRESULT CMouse_Manager::Initialize(HINSTANCE hInst, HWND hWnd)
{
	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<void**>(&m_pInputSDK),
		NULL)))
		return E_FAIL;

	// 마우스 객체 생성 및 할당.
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	m_pMouse->SetDataFormat(&c_dfDIMouse);
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pMouse->Acquire();

	return S_OK;
}

void CMouse_Manager::Update()
{
	memcpy(&m_tPreMouseState, &m_tMouseState, sizeof(m_tMouseState));
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}

_bool CMouse_Manager::Mouse_Down(MOUSEKEYSTATE eMouse)
{
	return !m_tPreMouseState.rgbButtons[ENUM_CLASS(eMouse)] && m_tMouseState.rgbButtons[ENUM_CLASS(eMouse)];
}

_bool CMouse_Manager::Mouse_Up(MOUSEKEYSTATE eMouse)
{
	return m_tPreMouseState.rgbButtons[ENUM_CLASS(eMouse)] && !m_tMouseState.rgbButtons[ENUM_CLASS(eMouse)];
}

CMouse_Manager* CMouse_Manager::Create(HINSTANCE hInstance, HWND hWnd)
{
	CMouse_Manager* pInstance = new CMouse_Manager();

	if (FAILED(pInstance->Initialize(hInstance, hWnd)))
	{
		MSG_BOX("Create Failed : Mouse Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMouse_Manager::Free()
{
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}
