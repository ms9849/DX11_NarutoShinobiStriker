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

class CBoss_SlidingKunaiState final : public CBossState
{
public:
	enum class ANIM_STATE { LEFT, RIGHT, END };
private:
	CBoss_SlidingKunaiState(class CNavigation* pNavigation, class CBoss* pBoss, ANIM_STATE eAnimState);
	virtual ~CBoss_SlidingKunaiState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CBossState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CBoss*	m_pBoss = { nullptr };
	CNavigation*	m_pNavigationCom = { nullptr };
	_bool			m_IsKunaiThrow = { false };
	ANIM_STATE		m_eAnimState = {};

public:
	static CBoss_SlidingKunaiState* Create(class CNavigation* pNavigation, class CBoss* pBoss, ANIM_STATE eAnimState);
	virtual void Free() override;
};

NS_END