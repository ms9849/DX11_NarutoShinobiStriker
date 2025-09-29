#pragma once
#include "Client_Defines.h"
#include "PajamaState.h"

NS_BEGIN(Engine)
class CNavigation;
NS_END
/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Client)

class CPajama_ElectricShockState final : public CPajamaState
{
private:
	CPajama_ElectricShockState(CNavigation* pNavigation, class CPajama* pPajama);
	virtual ~CPajama_ElectricShockState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CPajamaState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	CNavigation* m_pNavigationCom = { nullptr };
	class CPajama* m_pPajama = { nullptr };
	_float3 m_vDirection = {};
	_float  m_fRatio = {};
	_float				m_fTimeAcc = { 0.f };
	_float				m_fMaxTimeAcc = { 2.5f };

public:
	static CPajama_ElectricShockState* Create(CNavigation* pNavigation, class CPajama* pPajama);
	virtual void Free() override;
};

NS_END