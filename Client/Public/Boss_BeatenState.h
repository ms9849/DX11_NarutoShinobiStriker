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

class CBoss_BeatenState final : public CBossState
{
private:
	CBoss_BeatenState(CNavigation* pNavigation, class CBoss* pBoss, _vector vDir, _float fRatio);
	virtual ~CBoss_BeatenState() = default;

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

public:
	static CBoss_BeatenState* Create(CNavigation* pNavigation, class CBoss* pBoss, _vector vDir, _float fRatio);
	virtual void Free() override;
};

NS_END