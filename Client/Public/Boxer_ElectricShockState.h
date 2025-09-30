#pragma once
#include "Client_Defines.h"
#include "BoxerState.h"

NS_BEGIN(Engine)
class CNavigation;
NS_END
/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)

class CBoxer_ElectricShockState final : public CBoxerState
{
private:
	CBoxer_ElectricShockState(CNavigation* pNavigation, class CBoxer* pBoxer);
	virtual ~CBoxer_ElectricShockState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CBoxerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	CNavigation*	m_pNavigationCom = { nullptr };
	class CBoxer*	m_pBoxer = { nullptr };
	_float3			m_vDirection = {};
	_float			m_fRatio = {};
	_float			m_fTimeAcc = { 0.f };
	_float			m_fMaxTimeAcc = { 2.5f };

public:
	static CBoxer_ElectricShockState* Create(CNavigation* pNavigation, class CBoxer* pBoxer);
	virtual void Free() override;
};

NS_END