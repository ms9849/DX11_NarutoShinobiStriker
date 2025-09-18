#pragma once
#include "Client_Defines.h"
#include "WhiteJetsuState.h"

/*
가장 기본적인 IDLE STATE
*/

NS_BEGIN(Engine)
class CTransform;
class CModel;
class CNavigation;
NS_END

NS_BEGIN(Client)

class CWhiteJetsu_BeatenState final : public CWhiteJetsuState
{
private:
	CWhiteJetsu_BeatenState(class CNavigation* pNavigation, class CWhiteJetsu* pJetsu, _vector vDirection, _float fRatio = 1.f);
	virtual ~CWhiteJetsu_BeatenState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CWhiteJetsuState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	CTransform* m_pPlayerTransformCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	class CWhiteJetsu* m_pJetsu = { nullptr };
	_float3		m_vDirection = {};
	_float		m_fRatio = { 1.f };

public:
	static CWhiteJetsu_BeatenState* Create(class CNavigation* pNavigation, class CWhiteJetsu* pJetsu, _vector vDirection, _float fRatio = 1.f);
	virtual void Free() override;
};

NS_END