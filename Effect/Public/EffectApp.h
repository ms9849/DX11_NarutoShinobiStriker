#pragma once

#include "Effect_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(EffectTool)

class CEffectApp final : public CBase
{
private:
	CEffectApp();
	virtual ~CEffectApp() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	HRESULT Ready_Default_Setting();
	HRESULT Start_Level(LEVEL eLevelID);

public:
	static CEffectApp* Create();
	virtual void Free() override;
};

NS_END



