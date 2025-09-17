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

class CWhiteJetsu_BeatenBlastedState final : public CWhiteJetsuState
{
private:
	CWhiteJetsu_BeatenBlastedState(class CNavigation* pNavigation, class CWhiteJetsu* pJetsu, _vector vDirection);
	virtual ~CWhiteJetsu_BeatenBlastedState() = default;

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
	_float3		m_vDirection = {};

public:
	static CWhiteJetsu_BeatenBlastedState* Create(class CNavigation* pNavigation, class CWhiteJetsu* pJetsu, _vector vDirection);
	virtual void Free() override;
};

NS_END