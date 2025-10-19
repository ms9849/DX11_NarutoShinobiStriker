#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)

class CPlayer_RasenganState final : public CPlayerState
{
public:
	enum class ANIM_STATE { ATTACK_START, ATTACK, ATTACK_END };

private:
	CPlayer_RasenganState(class CPlayer* pPlayer, class CRasengan* pRasengan);
	virtual ~CPlayer_RasenganState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CPlayerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CRasengan* m_pRasengan = { nullptr };
	class CGameManager* m_pGameManager = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };
	ANIM_STATE	   m_eAnimState = {};
	_float		   m_fTimeAcc = { 0.f };
public:
	static CPlayer_RasenganState* Create(class CPlayer* pPlayer, class CRasengan* pRasengan);
	virtual void Free() override;
};

NS_END