#pragma once

#include "Client_Defines.h"
#include "Panel.h"

NS_BEGIN(Client)

class CLogoPanel final : public CPanel
{
private:
	CLogoPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CLogoPanel(const CLogoPanel& rhs);
	virtual ~CLogoPanel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources() override;
	HRESULT Ready_LogoUIs();

public:
	static CLogoPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
