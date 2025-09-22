#pragma once
#include "Client_Defines.h"
#include "WhiteJetsuState.h"

NS_BEGIN(Engine)
class CTransform;
class CModel;
class CNavigation;
NS_END

NS_BEGIN(Client)

class CWhiteJetsu_DeadState final : public CWhiteJetsuState
{
private:
	CWhiteJetsu_DeadState(class CNavigation* pNavigation, class CWhiteJetsu* pJetsu);
	virtual ~CWhiteJetsu_DeadState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CWhiteJetsuState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	class CGameManager* m_pGameManager = { nullptr };
	CTransform* m_pPlayerTransformCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	class CWhiteJetsu* m_pJetsu = { nullptr };
	_float3		m_vDirection = {};
	_float		m_fRatio = { 1.f };

public:
	static CWhiteJetsu_DeadState* Create(class CNavigation* pNavigation, class CWhiteJetsu* pJetsu);
	virtual void Free() override;
};

NS_END