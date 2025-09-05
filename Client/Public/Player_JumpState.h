#pragma once

#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)

class CPlayer_JumpState final : public CPlayerState
{
public:
	enum class ANIM_STATE { JUMP, DOUBLE_JUMP, FALL };
private:
	CPlayer_JumpState(class CPlayer* pPlayer, _float fTimeAcc, ANIM_STATE eStartAnimState);
	virtual ~CPlayer_JumpState() = default;

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
	static CPlayer_JumpState* Create(class CPlayer* pPlayer, _float fTimeAcc = 0.f, ANIM_STATE eStartAnimState = ANIM_STATE::JUMP);
	virtual void Free() override;
};

NS_END