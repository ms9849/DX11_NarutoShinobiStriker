#pragma once

#include "Client_Defines.h"
#include "BirdState.h"

NS_BEGIN(Engine)
class CTransform;
class CModel;
class CNavigation;
NS_END

NS_BEGIN(Client)

class CBird_AttackState final : public CBirdState
{
private:
	CBird_AttackState(class CTransform* pTransform, class CNavigation* pNavigation, class CModel* pModelCom);
	virtual ~CBird_AttackState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CBirdState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	CTransform* m_pPlayerTransformCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };

public:
	static CBird_AttackState* Create(class CTransform* pTransform, class CNavigation* pNavigation, class CModel* pModelCom);
	virtual void Free() override;
};

NS_END