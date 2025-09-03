#pragma once

#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)

class CPlayer_RunState final : public CPlayerState
{
private:
	CPlayer_RunState(class CPlayer* pPlayer);
	virtual ~CPlayer_RunState() = default;

public:
	/* Start */
	virtual void	Start() override;
	/* Loop */
	virtual CPlayerState* Update(_float fTimeDelta) override;
	/* End */
	virtual void	End() override;

private:
	class CPlayer* m_pPlayer = { nullptr };

public:
	static CPlayer_RunState* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};

NS_END