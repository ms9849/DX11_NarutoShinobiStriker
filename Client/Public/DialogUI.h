#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CGameInstance; 
class CFont;
class CShader;
class CTexture;
NS_END 

NS_BEGIN(Client)

class CDialogUI : public CUIObject
{
private:
	CDialogUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CDialogUI(const CDialogUI& rhs);
	virtual ~CDialogUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_NameText(const _wstring& strText) { m_strNameFontText = strText; }
	void Set_Text(const _wstring& strText) { m_strFontText = strText; }
	void Set_Visible(_bool bFlag) { m_IsVisible = bFlag; }

private:
	class CGameManager* m_pGameManager = { nullptr };
	_bool	 m_IsVisible = { false };
	CFont*	 m_pFontCom = {};
	CFont*	 m_pNameFontCom = {};
	_uint	 m_iNumMeshes = {};
	_wstring m_strFontText = {};
	_wstring m_strNameFontText = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CDialogUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

NS_END
