#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer;
class CShader;
NS_END

NS_BEGIN(Client)

class CDecimalUI final : public CUIObject
{
public:
	enum class DECIMAL_TYPE { TIMER, COMBO, END };

	typedef struct tagDecimalUI : public UIOBJECT_DESC {
		DECIMAL_TYPE eDecimal;
	} DECIMAL_DESC;

private:
	CDecimalUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CDecimalUI(const CDecimalUI& rhs);
	virtual ~CDecimalUI() = default;

public:
	void Set_CurrentIdx(_uint iIdx) {
		m_iTextureIdx = ENUM_CLASS(m_eDecimalType) * 10 + iIdx;
	}

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

public:
	void AlertPanel_Start_FadeIn();
	void Start_FadeOut();

private:
	class CGameManager* m_pGameManager = { nullptr };
	DECIMAL_TYPE m_eDecimalType = {};
	_bool m_IsVisible = { true };

	_float m_fMaxScale = { 1.5f };
	/* 페이드 인*/
	_bool  m_IsFadeIn = { false };
	_float m_fFadeInTimeAcc = { 0.f };
	_float m_fFadeInMaxTimeAcc = { 0.2f };

	/* 페이드 아웃 */
	_bool  m_IsFadeOut = { false };
	_float m_fFadeOutTimeAcc = { 0.f };
	_float m_fFadeOutMaxTimeAcc = { 0.2f };

private:
	HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources() override;

private:
	void	Play_Animation_FadeIn(_float fTimeDelta);
	void	Play_Animation_FadeOut(_float fTimeDelta);

public:
	static CDecimalUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
