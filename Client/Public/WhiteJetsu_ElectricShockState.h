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

class CWhiteJetsu_ElectricShockState final : public CWhiteJetsuState
{
private:
	CWhiteJetsu_ElectricShockState(class CNavigation* pNavigation, class CWhiteJetsu* pJetsu);
	virtual ~CWhiteJetsu_ElectricShockState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CWhiteJetsuState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CGameManager* m_pGameManager = { nullptr };
	CNavigation*		m_pNavigationCom = { nullptr };
	class CWhiteJetsu*	m_pJetsu = { nullptr };
	_float				m_fTimeAcc = { 0.f };
	_float				m_fMaxTimeAcc = { 2.5f };

public:
	static CWhiteJetsu_ElectricShockState* Create(class CNavigation* pNavigation, class CWhiteJetsu* pJetsu);
	virtual void Free() override;
};

NS_END