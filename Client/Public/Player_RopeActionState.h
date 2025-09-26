#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)

class CPlayer_RopeActionState final : public CPlayerState
{
public:
	/* 
	로프를 던지는 중인 THROW 상태,
	로프
	*/
	enum class ANIM_STATE { THROW, FLY, END };
private:
	CPlayer_RopeActionState(class CPlayer* pPlayer, _vector vDir, _vector vPosition);
	virtual ~CPlayer_RopeActionState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CPlayerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CPlayer* m_pPlayer = { nullptr };
	_float3 m_vTargetDir = {};
	_float3 m_vTargetPos = {};
public:
	static CPlayer_RopeActionState* Create(class CPlayer* pPlayer, _vector vDir, _vector vPosition);
	virtual void Free() override;
};

NS_END