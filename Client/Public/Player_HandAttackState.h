#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)

class CPlayer_HandAttackState final : public CPlayerState
{
public:
	enum class ANIM_STATE{ LEFT_PUNCH, STRAIGHT_PUNCH, KNEE_KICK, JACK_CUT, DRAGON_KICK };
private:
	CPlayer_HandAttackState(class CPlayer* pPlayer);
	virtual ~CPlayer_HandAttackState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CPlayerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CPlayer* m_pPlayer = { nullptr };
	ANIM_STATE m_eAnimState = {};

public:
	static CPlayer_HandAttackState* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};

NS_END