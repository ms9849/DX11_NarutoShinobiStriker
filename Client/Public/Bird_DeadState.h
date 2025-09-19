#pragma once

#include "Client_Defines.h"
#include "BirdState.h"

NS_BEGIN(Engine)
class CTransform;
class CModel;
class CNavigation;
NS_END

NS_BEGIN(Client)

class CBird_DeadState final : public CBirdState
{
private:
	CBird_DeadState(class CNavigation* pNavigation, class CBird* pBird);
	virtual ~CBird_DeadState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CBirdState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	CTransform* m_pPlayerTransformCom = { nullptr };
	class CBird* m_pBird = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	_float m_fTimeAcc = { 0.f };

public:
	static CBird_DeadState* Create(class CNavigation* pNavigation, class CBird* pBird);
	virtual void Free() override;
};

NS_END