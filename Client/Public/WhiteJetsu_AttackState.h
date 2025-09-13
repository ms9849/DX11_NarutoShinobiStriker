#pragma once

#include "Client_Defines.h"
#include "WhiteJetsuState.h"

NS_BEGIN(Engine)
class CModel;
class CTransform;
class CNavigation;
NS_END

NS_BEGIN(Client)

class CWhiteJetsu_AttackState final : public CWhiteJetsuState
{
private:
	CWhiteJetsu_AttackState(class CNavigation* pNavigation, class CWhiteJetsu* pJetsu);
	virtual ~CWhiteJetsu_AttackState() = default;

public:
	/* Start */
	void	Start(_bool IsBlend) override;
	/* Loop */
	CWhiteJetsuState* Update(_float fTimeDelta) override;
	/* End */
	_bool	End() override;

private:
	CNavigation* m_pNavigationCom = { nullptr };
	class CWhiteJetsu* m_pJetsu = { nullptr };

public:
	static CWhiteJetsu_AttackState* Create(class CNavigation* pNavigation, class CWhiteJetsu* pJetsu);
	virtual void Free() override;
};

NS_END