#pragma once
#include "Client_Defines.h"
#include "BoxerState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)

class CBoxer_IdleState final : public CBoxerState
{
private:
	CBoxer_IdleState(class CTransform* pTransform, class CNavigation* pNavigation, class CModel* pModelCom);
	virtual ~CBoxer_IdleState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CBoxerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	CTransform* m_pPlayerTransformCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };

public:
	static CBoxer_IdleState* Create(class CTransform* pTransform, class CNavigation* pNavigation, class CModel* pModelCom);
	virtual void Free() override;
};

NS_END