#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)

class CPlayer_BigSharkState final : public CPlayerState
{
private:
	CPlayer_BigSharkState(class CPlayer* pPlayer);
	virtual ~CPlayer_BigSharkState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CPlayerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CGameManager* m_pGameManager = { nullptr };
	class CPlayer*		m_pPlayer = { nullptr };
	_bool				m_IsBigSharkThrow = { false };
	_bool				m_IsCamChanged = { false };

public:
	static CPlayer_BigSharkState* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};

NS_END