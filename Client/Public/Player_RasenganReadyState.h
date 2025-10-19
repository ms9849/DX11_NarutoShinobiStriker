#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)

class CPlayer_RasenganReadyState final : public CPlayerState
{
private:
	CPlayer_RasenganReadyState(class CPlayer* pPlayer);
	virtual ~CPlayer_RasenganReadyState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CPlayerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CPlayer*	m_pPlayer = { nullptr };
	class CRasengan* m_pRasengan = { nullptr };
public:
	static CPlayer_RasenganReadyState* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};

NS_END