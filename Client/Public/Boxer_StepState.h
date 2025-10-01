#pragma once
#include "Client_Defines.h"
#include "BoxerState.h"

NS_BEGIN(Engine)
class CNavigation;
NS_END 

NS_BEGIN(Client)

class CBoxer_StepState final : public CBoxerState
{
public:
	enum class ANIM_STATE { LEFT, RIGHT, BACK, FRONT };

private:
	CBoxer_StepState(CNavigation* pNavigationCom, class CBoxer* pBoxer, ANIM_STATE eAnimState);
	virtual ~CBoxer_StepState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CBoxerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	CNavigation* m_pNavigationCom = { nullptr };
	class CBoxer* m_pBoxer = { nullptr };
	ANIM_STATE m_eAnimState = { };

public:
	static CBoxer_StepState* Create(CNavigation* pNavigationCom, class CBoxer* pBoxer, ANIM_STATE eAnimState);
	virtual void Free() override;
};

NS_END