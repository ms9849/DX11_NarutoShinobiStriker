#pragma once

/*
게임의 흐름 진행에 관여하는 매니저.

튜토리얼 맵 -> 본 맵으로의 이동도 담당할 수 있게 처리한다.

필요한 기능
0. 카카시랑 대화 시작한 뒤 스포너 생성.
1. 튜토리얼 -> 본 맵으로 넘어가는 순간
-> 트리거박스가 사라질떄 (Check Tutorial Clear) 체크 시작
-> 체크 시작한 이후 맵에 모든 몬스터가 사라졌다면

2. 본 맵 트리거 박스 생성
3. 대사 생성
*/

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CPlayer_Manager final : public CBase
{
private:
	CPlayer_Manager();
	virtual ~CPlayer_Manager() = default;

public:
	class CPlayer* Get_PlayerPtr() { return m_pPlayer; }
	HRESULT Set_PlayerPtr(class CPlayer* pPlayer);

public:
	/* 아웃핏 목록들 다 가져옴. */
	vector<pair<_wstring, _wstring>>& Get_Outfits(SELECT_TYPE eType) { return m_OutFits[ENUM_CLASS(eType)]; }
	void Add_Outfit(SELECT_TYPE eType, const _wstring& strOutfitTag, const _wstring& strModelTag);
	void Set_PlayerModelInfo(SELECT_TYPE eType, _uint iModelNum);
	_wstring Get_PlayerModelInfo(SELECT_TYPE eType);
	_bool IsOneCloth() { return m_IsOneCloth; }
public:
	void	Clear();

private:
	class CGameManager* m_pGameManager = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	class CPlayer* m_pPlayer = {};
	
	vector<pair<_wstring, _wstring>> m_OutFits[ENUM_CLASS(SELECT_TYPE::END)] = {};
	_uint m_ModelNums[ENUM_CLASS(SELECT_TYPE::END)] = {};
	_bool m_IsOneCloth = { false };

public:
	static CPlayer_Manager* Create();
	virtual void Free() override;
};

NS_END
