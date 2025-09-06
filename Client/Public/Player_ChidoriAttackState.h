#pragma once

#include "Client_Defines.h"
#include "PlayerState.h"

NS_BEGIN(Client)

class CPlayer_ChidoriAttackState final : public CPlayerState
{
public:
	enum class ANIM_STATE { ATTACK, ATTACK_END };
private:
	CPlayer_ChidoriAttackState(class CPlayer* pPlayer);
	virtual ~CPlayer_ChidoriAttackState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CPlayerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CPlayer* m_pPlayer = { nullptr };
	ANIM_STATE	   m_eAnimState = {};
	_float		   m_fTimeAcc = { 0.f };
public:
	static CPlayer_ChidoriAttackState* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};

NS_END