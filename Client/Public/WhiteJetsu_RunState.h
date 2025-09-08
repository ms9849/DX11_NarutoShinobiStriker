#pragma once

#include "Client_Defines.h"
#include "WhiteJetsuState.h"

NS_BEGIN(Engine)
class CTransform;
class CModel;
NS_END 

NS_BEGIN(Client)

class CWhiteJetsu_RunState final : public CWhiteJetsuState
{
private:
	CWhiteJetsu_RunState(class CTransform* pTransform, class CModel* pModelCom);
	virtual ~CWhiteJetsu_RunState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CWhiteJetsuState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	CTransform* m_pPlayerTransformCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

public:
	static CWhiteJetsu_RunState* Create(class CTransform* pTransform, class CModel* pModelCom);
	virtual void Free() override;
};

NS_END