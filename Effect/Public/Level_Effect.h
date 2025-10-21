#pragma once

#include "Effect_Defines.h"
#include "Level.h"

NS_BEGIN(Engine)
class CGameObject;
class CTransform;
class CNavigation;
class CCell;
NS_END

NS_BEGIN(EffectTool)

class CLevel_Effect final : public CLevel
{
private:
	CLevel_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CLevel_Effect() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Lights();
	HRESULT Ready_Prototypes();
	HRESULT Ready_FBXs();
	HRESULT Ready_Binarys();
	HRESULT Ready_Camera();

private:
	class CEffect_GUI* m_pEffectGUI = { nullptr };
	vector<pair<_wstring, _wstring>> m_strFBXModelTags = {};
	_float m_fTimeDelta = { 0.f };

private:


public:
	static CLevel_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual void Free() override;
};

NS_END
