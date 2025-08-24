#pragma once

#include "Editor_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Editor)

class CEditApp final : public CBase
{
private:
	CEditApp();
	virtual ~CEditApp() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	HRESULT Ready_Default_Setting();
	HRESULT Start_Level(LEVEL eLevelID);

public:
	static CEditApp* Create();
	virtual void Free() override;
};

NS_END



