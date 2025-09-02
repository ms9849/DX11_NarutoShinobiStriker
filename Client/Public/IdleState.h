#pragma once
#include "Client_Defines.h"
#include "PlayerState.h"

NS_BEGIN(Client)

class CIdleState final : public CPlayerState
{
private:
	CIdleState();
	virtual ~CIdleState() = default;

public:
	virtual void	Key_Input() override;
	/* Start */
	virtual void	Start(_float fTimeDelta) override;
	/* Loop */
	virtual void	Update(_float fTimeDelta) override;
	/* End */
	virtual void	End(_float fTimeDelta) override;

public:
	virtual void Free() override;
};

NS_END