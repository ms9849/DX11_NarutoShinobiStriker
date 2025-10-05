#pragma once

#include "Client_Defines.h"
#include "Panel.h"

NS_BEGIN(Engine)
class CFont;
NS_END

/* 이 UI들은 보스가 들고 있게 해야할 듯? */

NS_BEGIN(Client)

class CBossHPPanel final : public CPanel
{
private:
	CBossHPPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CBossHPPanel(const CBossHPPanel& rhs);
	virtual ~CBossHPPanel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Progress(_float fProgress);
	void Set_MaxProgress(_float fMaxProgress);
	void Set_Visible(_bool bFlag) { m_IsVisible = false;  }
private:
	_bool  m_IsVisible = { true };

private:
	HRESULT Ready_ProgressBar();
	HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CBossHPPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END