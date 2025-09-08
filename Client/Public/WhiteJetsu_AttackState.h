#pragma once

#include "Client_Defines.h"
#include "WhiteJetsuState.h"

NS_BEGIN(Engine)
class CModel;
class CTransform;
NS_END

NS_BEGIN(Client)

class CWhiteJetsu_AttackState final : public CWhiteJetsuState
{
private:
	CWhiteJetsu_AttackState(class CTransform* pTransform, class CModel* pModelCom);
	virtual ~CWhiteJetsu_AttackState() = default;

public:
	/* Start */
	void	Start(_bool IsBlend) override;
	/* Loop */
	CWhiteJetsuState* Update(_float fTimeDelta) override;
	/* End */
	_bool	End() override;

private:
	class CTransform* m_pTransformCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };
public:
	static CWhiteJetsu_AttackState* Create(class CTransform* pTransform, class CModel* pModelCom);
	virtual void Free() override;
};

NS_END