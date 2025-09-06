#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)

class CPlayer_SuperSharkState final : public CPlayerState
{
private:
	CPlayer_SuperSharkState(class CPlayer* pPlayer);
	virtual ~CPlayer_SuperSharkState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CPlayerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CPlayer* m_pPlayer = { nullptr };
public:
	static CPlayer_SuperSharkState* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};

NS_END