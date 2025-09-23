#pragma once
#include "Client_Defines.h"
#include "BirdState.h"

/*
가장 기본적인 IDLE STATE
*/

NS_BEGIN(Engine)
class CTransform;
class CModel;
class CNavigation;
NS_END

NS_BEGIN(Client)

class CBird_ElectricShockState final : public CBirdState
{
private:
	CBird_ElectricShockState(class CNavigation* pNavigation, class CBird* pBird);
	virtual ~CBird_ElectricShockState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CBirdState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CGameManager* m_pGameManager = { nullptr };
	CTransform* m_pPlayerTransformCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	class CBird* m_pBird = { nullptr };
	_float				m_fTimeAcc = { 0.f };
	_float				m_fMaxTimeAcc = { 2.5f };

public:
	static CBird_ElectricShockState* Create(class CNavigation* pNavigation, class CBird* pBird);
	virtual void Free() override;
};

NS_END