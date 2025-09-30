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

class CBoxer_BeatenBlastedState final : public CBoxerState
{
private:
	CBoxer_BeatenBlastedState(CNavigation* pNavigation, class CBoxer* pBoxer, _vector vDir, _float fRatio);
	virtual ~CBoxer_BeatenBlastedState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CBoxerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	CNavigation* m_pNavigationCom = { nullptr };
	class CBoxer* m_pBoxer = { nullptr };
	_float3 m_vDirection = {};
	_float  m_fRatio = {};

public:
	static CBoxer_BeatenBlastedState* Create(CNavigation* pNavigation, class CBoxer* pBoxer, _vector vDir, _float fRatio);
	virtual void Free() override;
};

NS_END