#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CPlayerState abstract : public CBase
{
protected:
	CPlayerState();
	virtual ~CPlayerState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) = 0;
	/* Loop */
	virtual CPlayerState* Update(_float fTimeDelta) = 0;
	/* End */
	virtual _bool	End() = 0;

protected:
	CGameInstance* m_pGameInstance = { nullptr };
	_bool		   m_IsNextAnimBlened = { true }; 

public:
	virtual void Free() override;
};

NS_END

