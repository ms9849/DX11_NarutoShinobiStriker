#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CBirdState abstract : public CBase
{
protected:
	CBirdState();
	virtual ~CBirdState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) = 0;
	/* Loop */
	virtual CBirdState* Update(_float fTimeDelta) = 0;
	/* End */
	virtual _bool	End() = 0;

protected:
	CGameInstance* m_pGameInstance = { nullptr };
	_bool		   m_IsNextAnimBlened = { true };

public:
	virtual void Free() override;
};

NS_END

