#pragma once

#include "Client_Defines.h"
#include "Panel.h"

NS_BEGIN(Engine)
class CFont;
NS_END

NS_BEGIN(Client)

class CWinPanel final : public CPanel
{
private:
	CWinPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CWinPanel(const CWinPanel& rhs);
	virtual ~CWinPanel() = default;

public:
	void WinPanel_Start_FadeIn();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float m_fMaxScale = { 1.5f };
	_bool  m_IsFadeIn = { false };
	_float m_fFadeInTimeAcc = { 0.f };
	_float m_fFadeInMaxTimeAcc = { 0.3f };

	_bool  m_IsVisible = { false };
	_bool  m_IsFadeInFinished = { false };

private:
	HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources() override;
	void	Play_Animation_FadeIn(_float fTimeDelta);

public:
	static CWinPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END