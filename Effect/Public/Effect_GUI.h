#pragma once

#include "Effect_Defines.h"
#include "Base.h"

NS_BEGIN(EffectTool)

/*
이번엔 레벨에 붙이지말고 ㅇㅇ
*/

class CEffect_GUI final : public CBase
{
private:
	CEffect_GUI();
	virtual ~CEffect_GUI() = default;

public:
	HRESULT Initialize();

private:

public:
	static CEffect_GUI* Create();
	virtual void Free() override;
};

NS_END