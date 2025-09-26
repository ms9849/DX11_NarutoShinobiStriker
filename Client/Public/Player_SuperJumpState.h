#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)

class CPlayer_SuperJumpState final : public CPlayerState
{
public:
	enum class ANIM_STATE { START, JUMP, DOUBLE_JUMP, FALL };
private:
	CPlayer_SuperJumpState(class CPlayer* pPlayer, _float fPower);
	virtual ~CPlayer_SuperJumpState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CPlayerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CPlayer* m_pPlayer = { nullptr };
	_float m_fTimeAcc = { 0.f };
	ANIM_STATE m_eAnimState = {};
	_float m_fPower = { 0.f };
	_float m_fMovement = {};
	_bool  m_IsAnimLoopFinished = { false };
	_bool  m_IsTriggered = { false };
public:
	static CPlayer_SuperJumpState* Create(class CPlayer* pPlayer, _float fPower);
	virtual void Free() override;
};

NS_END