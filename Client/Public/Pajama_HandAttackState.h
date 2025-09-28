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

class CPajama_HandAttackState final : public CPajamaState
{
private:
	CPajama_HandAttackState(class CNavigation* pNavigation, class CPajama* pPajama);
	virtual ~CPajama_HandAttackState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CPajamaState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

	void Update_Collider(_float fAnimProgress);
private:
	CPajama* m_pPajama = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	_bool m_IsOnCollider = { false };
public:
	static CPajama_HandAttackState* Create(class CNavigation* pNavigation, class CPajama* pPajama);
	virtual void Free() override;
};

NS_END