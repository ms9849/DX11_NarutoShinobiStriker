#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)

class CPlayer_DuckingState final : public CPlayerState
{
public:
	enum class ANIM_STATE { START, LOOP, END };
private:
	CPlayer_DuckingState(class CPlayer* pPlayer);
	virtual ~CPlayer_DuckingState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CPlayerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CEffectContainer* m_pEffectMain = { nullptr };
	class CPlayer*	m_pPlayer = { nullptr };
	ANIM_STATE		m_eAnimState = {};
	_float			m_fTimeAcc = { 0.f };
	_bool			m_IsChargeEnd = { false };

public:
	static CPlayer_DuckingState* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};

NS_END