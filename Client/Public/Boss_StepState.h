#pragma once
#include "Client_Defines.h"
#include "BossState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Engine)
class CNavigation;
NS_END

NS_BEGIN(Client)

class CBoss_StepState final : public CBossState
{
public:
	enum class ANIM_STATE { LEFT, RIGHT, BACK, FRONT };

private:
	CBoss_StepState(CNavigation* pNavigationCom, class CBoss* pBoss, ANIM_STATE eAnimState);
	virtual ~CBoss_StepState() = default;

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
	ANIM_STATE m_eAnimState = { };

public:
	static CBoss_StepState* Create(CNavigation* pNavigationCom, class CBoss* pBoss, ANIM_STATE eAnimState);
	virtual void Free() override;
};

NS_END