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
	enum class ANIM_STATE{ LEFT_PUNCH, STRAIGHT_PUNCH, KICK, BACK_KICK };
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

	void Update_Collider(_float fAnimProgress);
private:
	class CGameManager* m_pGameManager = { nullptr };
	class CPlayer*	m_pPlayer = { nullptr };
	ANIM_STATE		m_eAnimState = {};
	_bool			m_IsOnCollider = { false };


public:
	static CPlayer_HandAttackState* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};

NS_END