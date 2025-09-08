#pragma once
#include "Client_Defines.h"
#include "WhiteJetsuState.h"

/*
가장 기본적인 IDLE STATE
*/

NS_BEGIN(Engine)
class CTransform;
class CModel;
NS_END

NS_BEGIN(Client)

class CWhiteJetsu_IdleState final : public CWhiteJetsuState
{
private:
	CWhiteJetsu_IdleState(class CTransform* pTransform, class CModel* pModelCom);
	virtual ~CWhiteJetsu_IdleState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CWhiteJetsuState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	CTransform* m_pPlayerTransformCom = { nullptr };
	CTransform*	m_pTransformCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };

public:
	static CWhiteJetsu_IdleState* Create(class CTransform* pTransform, class CModel* pModelCom);
	virtual void Free() override;
};

NS_END