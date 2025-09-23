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
	void Toggle_ShowNormal() { 
		m_bShowNormal = !m_bShowNormal; 
	}

	void Set_Visible(_bool bFlag) {
		m_IsVisible = bFlag;
	}

	_bool IsPlaying() {
		return m_IsFadeIn || m_IsFadeOut;
	}

public:
	_bool	IsClicked();
	_bool	IsHovered();
	void	Toggle_Focus();
	/* 추후 폰트도 입력받게 하기.. */
	void	Trigger_FadeIn(_float fReserveTime);
	void	Trigger_FadeOut(_float fReserveTime);
	void	Play_Animation_FadeOut(_float fTimeDelta);
	void	Play_Animation_FadeIn(_float fTimeDelta);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	RECT m_rcButton = {};
	_bool m_IsVisible = { true };

	_float m_fReserveTime = { 0.f };
	_float m_fTimeAcc = { 0.f };

	_float m_fAnimationDist = { 50.f };
	_float m_fFadeOutTimeAcc = { 0.f };
	_float m_fFadeOutMaxTimeAcc = { 0.5f };
	_bool  m_IsFadeOut = { false };

	_float m_fFadeInTimeAcc = { 0.f };
	_float m_fFadeInMaxTimeAcc = { 0.5f };
	_bool  m_IsFadeIn = { false };

	_bool  m_bShowNormal = { false };
protected:
	virtual HRESULT Bind_ShaderResources() override;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END
