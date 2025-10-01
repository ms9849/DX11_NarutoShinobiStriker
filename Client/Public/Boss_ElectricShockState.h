#pragma once
#include "Client_Defines.h"
#include "BossState.h"

NS_BEGIN(Engine)
class CNavigation;
NS_END
/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)

class CBoss_ElectricShockState final : public CBossState
{
private:
	CBoss_ElectricShockState(CNavigation* pNavigation, class CBoss* pBoss);
	virtual ~CBoss_ElectricShockState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CBossState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	CNavigation* m_pNavigationCom = { nullptr };
	class CBoss* m_pBoss = { nullptr };
	_float3		 m_vDirection = {};
	_float		 m_fRatio = {};
	_float		 m_fTimeAcc = { 0.f };
	_float		 m_fMaxTimeAcc = { 2.5f };

public:
	static CBoss_ElectricShockState* Create(CNavigation* pNavigation, class CBoss* pBoss);
	virtual void Free() override;
};

NS_END