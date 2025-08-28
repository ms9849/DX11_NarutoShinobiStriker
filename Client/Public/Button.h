#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CVIBuffer;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Client)

class CButton abstract : public CUIObject
{
protected:
	CButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CButton(const CButton& rhs);
	virtual ~CButton() = default;

public:
	void Set_Visible(_bool bFlag) {
		m_bVisible = bFlag;
	}

	void Trigger_FadeOut();

public:
	_bool IsClicked();
	_bool IsHovered();
	void  Toggle_Focus();
	void  Play_Animation_FadeOut(_float fTimeDelta);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	RECT m_rcButton = {};
	_bool m_bVisible = { true };
	_float m_fFadeOutTimeAcc = { 0.f };
	_float m_fFadeOutMaxTimeAcc = { 0.5f };
	_bool m_bFadeOut = { false };

protected:
	virtual HRESULT Bind_ShaderResources() override;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END
