#pragma once
#include "Client_Defines.h"
#include "PajamaState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Engine)
class CTransform;
class CNavigation;
class CModel;
NS_END

NS_BEGIN(Client)

class CPajama_FireballState final : public CPajamaState
{
private:
	CPajama_FireballState(class CNavigation* pNavigation, class CPajama* pPajama);
	virtual ~CPajama_FireballState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CPajamaState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	CPajama*		m_pPajama = { nullptr };
	CNavigation*	m_pNavigationCom = { nullptr };
	_bool			m_IsFireballThrow = { false };

public:
	static CPajama_FireballState* Create(class CNavigation* pNavigation, class CPajama* pPajama);
	virtual void Free() override;
};

NS_END