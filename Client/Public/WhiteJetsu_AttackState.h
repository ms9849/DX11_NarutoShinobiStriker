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
	CWhiteJetsu_AttackState(class CTransform* pTransform, class CNavigation* pNavigation, class CModel* pModelCom);
	virtual ~CWhiteJetsu_AttackState() = default;

public:
	/* Start */
	void	Start(_bool IsBlend) override;
	/* Loop */
	CWhiteJetsuState* Update(_float fTimeDelta) override;
	/* End */
	_bool	End() override;

private:
	CTransform* m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };

public:
	static CWhiteJetsu_AttackState* Create(class CTransform* pTransform, class CNavigation* pNavigation, class CModel* pModelCom);
	virtual void Free() override;
};

NS_END