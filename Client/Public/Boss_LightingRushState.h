#pragma once
#include "Client_Defines.h"
#include "BossState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Engine)
class CTransform;
class CNavigation;
class CModel;
NS_END

NS_BEGIN(Client)

class CBoss_LightingRushState final : public CBossState
{
public:
	enum class ANIM_STATE { START, LOOP, END };

private:
	CBoss_LightingRushState(class CNavigation* pNavigation, class CBoss* pBoss);
	virtual ~CBoss_LightingRushState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CBossState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CBoss* m_pBoss = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };

	ANIM_STATE	 m_eAnimState = {};
	_float4		 m_RushPoints[8] = {};
	_float		 m_fTimeAcc = { 0.f }; 
	_uint		 m_iRushCount = { 0 };

public:
	static CBoss_LightingRushState* Create(class CNavigation* pNavigation, class CBoss* pBoss);
	virtual void Free() override;
};

NS_END