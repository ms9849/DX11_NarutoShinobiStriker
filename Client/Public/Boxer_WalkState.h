#pragma once
#include "Client_Defines.h"
#include "BoxerState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Engine)
class CTransform;
class CNavigation;
class CModel;
NS_END

NS_BEGIN(Client)

class CBoxer_WalkState final : public CBoxerState
{
private:
	CBoxer_WalkState(class CNavigation* pNavigation, class CBoxer* pBoxer);
	virtual ~CBoxer_WalkState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CBoxerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	CBoxer* m_pBoxer = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };

public:
	static CBoxer_WalkState* Create(class CNavigation* pNavigation, class CBoxer* pBoxer);
	virtual void Free() override;
};

NS_END