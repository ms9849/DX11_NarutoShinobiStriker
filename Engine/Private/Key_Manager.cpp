#include "Key_Manager.h"

CKey_Manager::CKey_Manager()
{
	ZeroMemory(&m_bKeyState, VK_MAX);
	ZeroMemory(&m_bPreKeyState, VK_MAX);
}

HRESULT CKey_Manager::Initialize()
{
	return S_OK;
}

void CKey_Manager::Key_Input()
{
	ZeroMemory(&m_bKeyState, VK_MAX);

	for (_int i = 0; i < VK_MAX; ++i)
	{
		if (GetAsyncKeyState(i) & 0x8000)
			m_bKeyState[i] = true;
	}
}

// 이전 프레임도 누르고, 지금 프레임도 눌렀을 때
_bool CKey_Manager::Key_Pressing(_uint _iKey)
{
	return m_bPreKeyState[_iKey] && m_bKeyState[_iKey];
}

// 이전 프레임에 눌린 적이 없고, 지금 막 눌렀을 때
_bool CKey_Manager::Key_Down(_uint _iKey)
{
	return !m_bPreKeyState[_iKey] && m_bKeyState[_iKey];
}

// 이전 프레임에 눌렀고, 지금은 누르지 않았을 때
_bool CKey_Manager::Key_Up(_uint _iKey)
{
	return m_bPreKeyState[_iKey] && !m_bKeyState[_iKey];
}

void CKey_Manager::Update()
{
	memcpy(m_bPreKeyState, m_bKeyState, VK_MAX);
}

CKey_Manager* CKey_Manager::Create()
{
	CKey_Manager* pInstance = new CKey_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CREATE FAILED: Key Manager");
	}

	return pInstance;
}

void CKey_Manager::Free()
{
	__super::Free();
}