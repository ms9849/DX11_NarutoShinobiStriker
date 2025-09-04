#pragma once

#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)



class CPlayer_FrontJumpState final : public CPlayerState
{
public:
	enum class ANIM_STATE { JUMP, DOUBLE_JUMP, FALL };
private:
	CPlayer_FrontJumpState(class CPlayer* pPlayer);
	virtual ~CPlayer_FrontJumpState() = default;

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
	_float	       m_fMovement = { 0.f };
	_float		   m_fSpeed = { 3.f };
	_float		   m_fTimeAcc = { 0.f };
	_bool		   m_bCanDoubleJump = false;
public:
	static CPlayer_FrontJumpState* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};

NS_END