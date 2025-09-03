#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

/* 
가장 기본적인 IDLE STATE 
*/
NS_BEGIN(Client)

class CPlayer_IdleState final : public CPlayerState
{
private:
	CPlayer_IdleState(class CPlayer* pPlayer);
	virtual ~CPlayer_IdleState() = default;

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
	static CPlayer_IdleState* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};

NS_END