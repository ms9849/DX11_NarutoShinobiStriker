#pragma once

#include "Client_Defines.h"
#include "PlayerState.h"
/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)

class CPlayer_SwordAttackState final : public CPlayerState
{
public:
	enum class ANIM_STATE { ATTACK_01, ATTACK_02, ATTACK_03 };

private:
	CPlayer_SwordAttackState(class CPlayer* pPlayer);
	virtual ~CPlayer_SwordAttackState() = default;

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
	static CPlayer_SwordAttackState* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};

NS_END