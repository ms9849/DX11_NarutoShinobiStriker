#pragma once

#include "GameObject.h"

/*
렌더러에 들어가기 위한 Update 계층 하나 / Render 외엔 

작동하지 않는 스태틱 오브젝트.
*/
NS_BEGIN(Engine)

class CStaticObject : public CGameObject
{
private:
	CStaticObject();
	CStaticObject();
	virtual ~CStaticObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:

public:

};

NS_END
