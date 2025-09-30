#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CTransform;
NS_END

NS_BEGIN(Client)

class CBoxerState abstract : public CBase
{
protected:
	CBoxerState();
	virtual ~CBoxerState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) = 0;
	/* Loop */
	virtual CBoxerState* Update(_float fTimeDelta) = 0;
	/* End */
	virtual _bool	End() = 0;

protected:
	class CGameManager* m_pGameManager = { nullptr };
	CGameInstance*		m_pGameInstance = { nullptr };
	CTransform*			m_pPlayerTransformCom = { nullptr };
	_bool				m_IsNextAnimBlened = { true };

public:
	virtual void Free() override;
};

NS_END

