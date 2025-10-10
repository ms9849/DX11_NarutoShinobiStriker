#pragma once

#include "Client_Defines.h"
#include "Panel.h"

NS_BEGIN(Engine)
class CFont;
NS_END

NS_BEGIN(Client)

class CCutScenePanel final : public CPanel
{
private:
	CCutScenePanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CCutScenePanel(const CCutScenePanel& rhs);
	virtual ~CCutScenePanel() = default;

public:
	void Set_Visible(_bool bFlag) {
		m_IsVisible = bFlag;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CFont* m_pFontCom = { nullptr };
	CFont* m_pScriptFontCom = { nullptr };

	_bool  m_IsVisible = { true };
	_bool  m_IsActiveScript = { false };
	_wstring m_strFontText = {};
	_wstring m_strScriptFontText = {};
	
private:
	HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CCutScenePanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END