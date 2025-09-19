

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

class CBird_BeatenState final : public CBirdState
{
private:
	CBird_BeatenState(class CNavigation* pNavigation, class CBird* pBird, _vector vDirection, _float fRatio = 1.f);
	virtual ~CBird_BeatenState() = default;

public:
	/* Start */
	virtual void	Start(_bool IsBlend) override;
	/* Loop */
	virtual CBirdState* Update(_float fTimeDelta) override;
	/* End */
	virtual _bool	End() override;

private:
	CTransform* m_pPlayerTransformCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	class CBird* m_pBird = { nullptr };
	_float3		m_vDirection = {};
	_float		m_fRatio = { 1.f };

public:
	static CBird_BeatenState* Create(class CNavigation* pNavigation, class CBird* pBird, _vector vDirection, _float fRatio = 1.f);
	virtual void Free() override;
};

NS_END