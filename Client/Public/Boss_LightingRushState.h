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
	ANIM_STATE	 m_eAnimState = { };
	_uint		 m_iRushCount = { 0 };
	_float		 m_fTimeAcc = { 0.f };

	_float3		 m_vTargetPos = {};
	_float		 m_fTargetDist = {};
	_float3      m_RushDirs[2] = {};
	_int		 m_iRushDir = {};

public:
	static CBoss_LightingRushState* Create(class CNavigation* pNavigation, class CBoss* pBoss);
	virtual void Free() override;
};

NS_END