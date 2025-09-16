#pragma once
#include "Client_Defines.h"
#include "WhiteJetsuState.h"

/*
가장 기본적인 IDLE STATE
*/

NS_BEGIN(Engine)
class CTransform;
class CModel;
class CNavigation;
NS_END

NS_BEGIN(Client)

class CWhiteJetsu_BeatenState final : public CWhiteJetsuState
{
private:
	CWhiteJetsu_BeatenState(class CNavigation* pNavigation, class CWhiteJetsu* pJetsu);
	virtual ~CWhiteJetsu_BeatenState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CWhiteJetsuState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	CTransform* m_pPlayerTransformCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	class CWhiteJetsu* m_pJetsu = { nullptr };

public:
	static CWhiteJetsu_BeatenState* Create(class CNavigation* pNavigation, class CWhiteJetsu* pJetsu);
	virtual void Free() override;
};

NS_END