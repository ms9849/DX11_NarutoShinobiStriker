#pragma once

#include "Client_Defines.h"
#include "Panel.h"

NS_BEGIN(Client)

class CMissionAlertPanel final : public CPanel
{
private:
	CMissionAlertPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CMissionAlertPanel(const CMissionAlertPanel& rhs);
	virtual ~CMissionAlertPanel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float m_fAnimationDist = 20.f;

	_bool  m_bFadeIn = { false };
	_float m_fFadeInTimeAcc = { 0.f };
	_float m_fFadeInMaxTimeAcc = { 0.3f };

	_bool  m_bTriggered = { false };
	_bool  m_bFadeOut = { false };
	_float m_fFadeOutTimeAcc = { 0.f };
	_float m_fFadeOutMaxTimeAcc = { 0.3f };

	/* Fade IN 끝난뒤, 수명 체크용 */
	_float m_fTimeAcc = 0.f;
	_float m_fLifeTime = 10.f;
	_bool  m_bVisible = { true };

private:
	HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources() override;
	void	Play_Animation_FadeIn(_float fTimeDelta);
	void	Play_Animation_FadeOut(_float fTimeDelta);

public:
	static CMissionAlertPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END