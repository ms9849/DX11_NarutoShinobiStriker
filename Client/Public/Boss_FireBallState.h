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

class CBoss_FireBallState final : public CBossState
{
private:
	CBoss_FireBallState(class CNavigation* pNavigation, class CBoss* pBoss);
	virtual ~CBoss_FireBallState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CBossState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	CBoss*			m_pBoss = { nullptr };
	CNavigation*	m_pNavigationCom = { nullptr };
	_bool			m_IsFireballThrow = { false };
	_int			m_iFireBallCount = { 0 };
	_float			m_fTimeAcc = { 0.f };
public:
	static CBoss_FireBallState* Create(class CNavigation* pNavigation, class CBoss* pBoss);
	virtual void Free() override;
};

NS_END