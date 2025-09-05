#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)

class CPlayer_HandAerialAttackState final : public CPlayerState
{
public:
	enum class ANIM_STATE { ATTACK_01, ATTACK_02 };
private:
	CPlayer_HandAerialAttackState(class CPlayer* pPlayer, _float fTimeAcc);
	virtual ~CPlayer_HandAerialAttackState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CPlayerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CPlayer*	m_pPlayer = { nullptr };
	ANIM_STATE		m_eAnimState = {};
	_float			m_fTimeAcc =  {};
	_float			m_fMovement = {};

public:
	static CPlayer_HandAerialAttackState* Create(class CPlayer* pPlayer, _float fTimeAcc);
	virtual void Free() override;
};

NS_END