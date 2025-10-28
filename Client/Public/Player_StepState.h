#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)

class CPlayer_StepState final : public CPlayerState
{
public:
	enum class ANIM_STATE { LEFT, RIGHT, BACK, FRONT };

private:
	CPlayer_StepState(class CPlayer* pPlayer, ANIM_STATE eAnimState);
	virtual ~CPlayer_StepState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CPlayerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CPlayer*	m_pPlayer = { nullptr };
	ANIM_STATE		m_eAnimState = { };
	_bool			m_IsCreatedEffect = { false };
public:
	static CPlayer_StepState* Create(class CPlayer* pPlayer, ANIM_STATE eAnimState);
	virtual void Free() override;
};

NS_END