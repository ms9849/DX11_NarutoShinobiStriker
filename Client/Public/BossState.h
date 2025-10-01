#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CTransform;
NS_END

NS_BEGIN(Client)

class CBossState abstract : public CBase
{
protected:
	CBossState();
	virtual ~CBossState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) = 0;
	/* Loop */
	virtual CBossState* Update(_float fTimeDelta) = 0;
	/* End */
	virtual _bool	End() = 0;

protected:
	CGameInstance*		m_pGameInstance = { nullptr };
	class CGameManager* m_pGameManager = { nullptr };
	CTransform*			m_pPlayerTransformCom = { nullptr };
	_bool				m_IsNextAnimBlened = { true };

public:
	virtual void Free() override;
};

NS_END

