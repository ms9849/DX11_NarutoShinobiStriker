#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CWhiteJetsuState abstract : public CBase
{
protected:
	CWhiteJetsuState();
	virtual ~CWhiteJetsuState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) = 0;
	/* Loop */
	virtual CWhiteJetsuState* Update(_float fTimeDelta) = 0;
	/* End */
	virtual _bool	End() = 0;

protected:
	CGameInstance* m_pGameInstance = { nullptr };
	_bool		   m_IsNextAnimBlened = { true };

public:
	virtual void Free() override;
};

NS_END

