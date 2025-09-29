#pragma once

#include "Client_Defines.h"
#include "PlayerState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)

class CPlayer_AerialFireBallState final : public CPlayerState
{
private:
	CPlayer_AerialFireBallState(class CPlayer* pPlayer, _float fTimeAcc);
	virtual ~CPlayer_AerialFireBallState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CPlayerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CPlayer* m_pPlayer = { nullptr };
	_float		   m_fTimeAcc = { 0.f };
	_float		   m_fCurMovement = { 0.f };
	_bool		   m_IsFloat = { false };
public:
	static CPlayer_AerialFireBallState* Create(class CPlayer* pPlayer, _float fTimeAcc);
	virtual void Free() override;
};

NS_END