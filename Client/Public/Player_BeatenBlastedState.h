#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)

class CPlayer_BeatenBlastedState final : public CPlayerState
{
private:
	CPlayer_BeatenBlastedState(class CPlayer* pPlayer, _vector vDir, _float fRatio);
	virtual ~CPlayer_BeatenBlastedState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CPlayerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CPlayer* m_pPlayer = { nullptr };
	_float3 m_vDirection = {};
	_float  m_fRatio = {};
public:
	static CPlayer_BeatenBlastedState* Create(class CPlayer* pPlayer, _vector vDir, _float fRatio);
	virtual void Free() override;
};

NS_END