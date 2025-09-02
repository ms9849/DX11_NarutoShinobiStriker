#pragma once

#include "Client_Defines.h"
#include "CharacterState.h"

NS_BEGIN(Client)

class CPlayerState : public CCharacterState
{
protected:
	CPlayerState();
	virtual ~CPlayerState() = default;

public:
	virtual void	Key_Input() = 0;
	/* Start */
	virtual void	Start(_float fTimeDelta) = 0;
	/* Loop */
	virtual void	Update(_float fTimeDelta) = 0;
	/* End */
	virtual void	End(_float fTimeDelta) = 0;

public:
	virtual void Free() = 0;
};

NS_END

