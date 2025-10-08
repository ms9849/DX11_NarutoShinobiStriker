#include "Player_Manager.h"

#include "Player.h"

CPlayer_Manager::CPlayer_Manager()
{
}

HRESULT CPlayer_Manager::Set_PlayerPtr(CPlayer* pPlayer)
{
	//인자로 받은 플레이어가 nullptr 이라면
	if (nullptr == pPlayer)
		return E_FAIL;

	//이미 플레이어가 등록되어 있다면,
	if (nullptr != m_pPlayer)
		Safe_Release(m_pPlayer);

	m_pPlayer = pPlayer;
	Safe_AddRef(m_pPlayer);

	return S_OK;
}

void CPlayer_Manager::Add_Outfit(SELECT_TYPE eType, const _wstring& strOutfitTag, const _wstring& strModelTag)
{
	m_OutFits[ENUM_CLASS(eType)].push_back(make_pair(strOutfitTag, strModelTag));
}

void CPlayer_Manager::Set_PlayerModelInfo(SELECT_TYPE eType, _uint iModelNum)
{
	if (SELECT_TYPE::ONE_CLOTH == eType)
		m_IsOneCloth = true;

	if (SELECT_TYPE::UPPER == eType || SELECT_TYPE::LOWER == eType)
		m_IsOneCloth = false;

	m_ModelNums[ENUM_CLASS(eType)] = iModelNum;
}

_wstring CPlayer_Manager::Get_PlayerModelInfo(SELECT_TYPE eType)
{
	return m_OutFits[ENUM_CLASS(eType)][m_ModelNums[ENUM_CLASS(eType)]].second;
}

void CPlayer_Manager::Clear()
{
	Safe_Release(m_pPlayer);

	if (nullptr != m_pPlayer)
	{
		m_pPlayer->Clear_State();
		m_pPlayer = nullptr;
	}
}

CPlayer_Manager* CPlayer_Manager::Create()
{
	return new CPlayer_Manager();
}

void CPlayer_Manager::Free()
{
	__super::Free();

	Clear();
}
