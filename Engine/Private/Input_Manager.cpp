#include "Input_Manager.h"

CInput_Manager::CInput_Manager()
{
}

HRESULT CInput_Manager::Initialize(HINSTANCE hInst, HWND hWnd)
{
	ZeroMemory(m_byKeyState, KEY_MAX);
	ZeroMemory(m_byPreKeyState, KEY_MAX);

	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<void**>(&m_pInputSDK),
		NULL)))
		return E_FAIL;

	// 키보드 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();

	// 마우스 객체 생성 및 할당.
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	m_pMouse->SetDataFormat(&c_dfDIMouse);
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pMouse->Acquire();

	return S_OK;
}

void CInput_Manager::Update()
{
	memcpy(m_byPreKeyState, m_byKeyState, KEY_MAX);
	m_pKeyBoard->GetDeviceState(KEY_MAX, m_byKeyState);

	memcpy(&m_tPreMouseState, &m_tMouseState, sizeof(m_tMouseState));
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}

_bool CInput_Manager::Key_Pressing(_ubyte byKey)
{
	return (m_byPreKeyState[byKey] & 0x80) && (m_byKeyState[byKey] & 0x80);
}

_bool CInput_Manager::Key_Down(_ubyte byKey)
{
	return !(m_byPreKeyState[byKey] & 0x80) && (m_byKeyState[byKey] & 0x80);
}

_bool CInput_Manager::Key_Up(_ubyte byKey)
{
	return (m_byPreKeyState[byKey] & 0x80) && !(m_byKeyState[byKey] & 0x80);
}

_bool CInput_Manager::Mouse_Down(MOUSEKEYSTATE eMouse)
{
	return !m_tPreMouseState.rgbButtons[ENUM_CLASS(eMouse)] && m_tMouseState.rgbButtons[ENUM_CLASS(eMouse)];
}

_bool CInput_Manager::Mouse_Up(MOUSEKEYSTATE eMouse)
{
	return m_tPreMouseState.rgbButtons[ENUM_CLASS(eMouse)] && !m_tMouseState.rgbButtons[ENUM_CLASS(eMouse)];
}

_bool CInput_Manager::Mouse_Pressing(MOUSEKEYSTATE eMouse)
{
	return m_tPreMouseState.rgbButtons[ENUM_CLASS(eMouse)] && m_tMouseState.rgbButtons[ENUM_CLASS(eMouse)];
}

CInput_Manager* CInput_Manager::Create(HINSTANCE hInstance, HWND hWnd)
{
	CInput_Manager* pInstance = new CInput_Manager();

	if (FAILED(pInstance->Initialize(hInstance, hWnd)))
	{
		MSG_BOX("Created Failed : Input Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInput_Manager::Free()
{
	Safe_Release(m_pMouse);
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pInputSDK);
}

