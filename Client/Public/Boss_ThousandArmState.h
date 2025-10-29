
#pragma once
#include "Client_Defines.h"
#include "BossState.h"

/*
가장 기본적인 IDLE STATE
*/
NS_BEGIN(Engine)
class CTransform;
class CNavigation;
class CModel;
NS_END

/* 캠 액션 추가해야한다.*/
NS_BEGIN(Client)

class CBoss_ThousandArmState final : public CBossState
{
private:
	CBoss_ThousandArmState(class CNavigation* pNavigation, class CBoss* pBoss);
	virtual ~CBoss_ThousandArmState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CBossState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CBoss* m_pBoss = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };

public:
	static CBoss_ThousandArmState* Create(class CNavigation* pNavigation, class CBoss* pBoss);
	virtual void Free() override;
};

NS_END