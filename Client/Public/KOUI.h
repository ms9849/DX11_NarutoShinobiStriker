#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer;
class CShader;
NS_END

NS_BEGIN(Client)

class CKOUI final : public CUIObject
{
private:
	CKOUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CKOUI(const CKOUI& rhs);
	virtual ~CKOUI() = default;

public:
	void Set_Visible(_bool bFlag) {
		m_bVisible = bFlag;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Start_FadeIn();
	void Start_FadeOut();

private:
	class CGameManager* m_pGameManager = { nullptr };
	_bool m_bVisible = { false };

	_float m_fMaxScale = { 1.5f };

	/* 페이드 인 이후 일정 시간이 지나면 자동으로 페이드 아웃 수행해주기 위함 */
	_bool m_bTriggered = { false };
	_float m_fTimeAcc = { 0.f };

	/* 페이드 인*/
	_bool  m_bFadeIn = { false };
	_float m_fFadeInTimeAcc = { 0.f };
	_float m_fFadeInMaxTimeAcc = { 0.2f };

	/* 페이드 아웃 */
	_bool  m_bFadeOut = { false };
	_float m_fFadeOutTimeAcc = { 0.f };
	_float m_fFadeOutMaxTimeAcc = { 0.2f };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	void	Play_Animation_FadeIn(_float fTimeDelta);
	void	Play_Animation_FadeOut(_float fTimeDelta);

public:
	static CKOUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
