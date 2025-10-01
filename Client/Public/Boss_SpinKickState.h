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

class CBoss_SpinKickState final : public CBossState
{
public:
	enum class ANIM_STATE { ATTACK_START, ATTACK_END };

private:
	CBoss_SpinKickState(class CNavigation* pNavigation, class CBoss* pBoss);
	virtual ~CBoss_SpinKickState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CBossState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

	void Update_Collider(_float fAnimProgress);

private:
	CBoss*			m_pBoss = { nullptr };
	CNavigation*	m_pNavigationCom = { nullptr };
	ANIM_STATE		m_eAnimState = {};
	_bool			m_IsOnCollider = { false };

public:
	static CBoss_SpinKickState* Create(class CNavigation* pNavigation, class CBoss* pBoss);
	virtual void Free() override;
};

NS_END