#pragma once
#include "Client_Defines.h"
#include "BoxerState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Engine)
class CTransform;
class CNavigation;
class CModel;
NS_END

NS_BEGIN(Client)

class CBoxer_AttackState final : public CBoxerState
{
public:
	enum class ANIM_STATE { ATTACK_01, ATTACK_02, ATTACK_03 };

private:
	CBoxer_AttackState(class CNavigation* pNavigation, class CBoxer* pBoxer);
	virtual ~CBoxer_AttackState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CBoxerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	CBoxer* m_pBoxer = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	ANIM_STATE m_eAnimState = {};

public:
	static CBoxer_AttackState* Create(class CNavigation* pNavigation, class CBoxer* pBoxer);
	virtual void Free() override;
};

NS_END