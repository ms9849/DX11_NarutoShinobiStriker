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

class CBoxer_SpinKickState final : public CBoxerState
{
public:
	enum class ANIM_STATE { ATTACK_START, ATTACK_END };

private:
	CBoxer_SpinKickState(class CNavigation* pNavigation, class CBoxer* pBoxer);
	virtual ~CBoxer_SpinKickState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CBoxerState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

	void Update_Collider(_float fAnimProgress);

private:
	CBoxer*			m_pBoxer = { nullptr };
	CNavigation*	m_pNavigationCom = { nullptr };
	ANIM_STATE		m_eAnimState = {};
	_bool			m_IsOnCollider = { false };
	_bool			m_IsParticleCreated = { false };
	_bool			m_IsSoundPlayed = { false };
public:
	static CBoxer_SpinKickState* Create(class CNavigation* pNavigation, class CBoxer* pBoxer);
	virtual void Free() override;
};

NS_END