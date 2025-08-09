#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CGameManager final : public CBase
{
	DECLARE_SINGLETON(CGameManager);

private:
	CGameManager();
	virtual ~CGameManager() = default;

public:
	HRESULT Initialize();
	void	Release_GameManager();

public:
	HRESULT Set_PlayerPtr(class CPlayer* pPlayer);
	HRESULT Set_QuestPtr(class CQuestLog* pQuestLog);
private:
	class CPlayer* m_pPlayer = {};
	class CQuestLog* m_pQuestLog = {};
	CGameInstance* m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};

NS_END

