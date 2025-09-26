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
	enum class ANIM_STATE { THROW, WAIT, ROPE, END };
private:
	CPlayer_RopeActionState(class CPlayer* pPlayer);
	virtual ~CPlayer_RopeActionState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CPlayerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CRope* m_pRope = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };
	_float3 m_vTargetDir = {};
	_float3 m_vTargetPos = {};
	ANIM_STATE m_eAnimState = {};

public:
	static CPlayer_RopeActionState* Create(class CPlayer* pPlayer);
	virtual void Free() override;
};

NS_END