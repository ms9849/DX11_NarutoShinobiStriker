#pragma once

#include "Client_Defines.h"
#include "PlayerState.h"

NS_BEGIN(Client)

class CPlayer_ChidoriAerialReadyState final : public CPlayerState
{
public:
	enum class ANIM_STATE { CHARGE_1, CHARGE_2, CHARGE_3 };
private:
	CPlayer_ChidoriAerialReadyState(class CPlayer* pPlayer, _float fTimeAcc);
	virtual ~CPlayer_ChidoriAerialReadyState() = default;

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
	_float		   m_fMovement = { 0.f };
public:
	static CPlayer_ChidoriAerialReadyState* Create(class CPlayer* pPlayer, _float fTimeAcc);
	virtual void Free() override;
};

NS_END