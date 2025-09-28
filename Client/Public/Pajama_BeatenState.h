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

class CPajama_BeatenState final : public CPajamaState
{
private:
	CPajama_BeatenState(CNavigation* pNavigation, class CPajama* pPajama, _vector vDir, _float fRatio);
	virtual ~CPajama_BeatenState() = default;

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

public:
	static CPajama_BeatenState* Create(CNavigation* pNavigation, class CPajama* pPajama, _vector vDir, _float fRatio);
	virtual void Free() override;
};

NS_END