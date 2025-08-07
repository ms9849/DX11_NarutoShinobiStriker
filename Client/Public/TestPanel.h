#pragma once

#include "Client_Defines.h"
#include "Panel.h"

NS_BEGIN(Client)

class CTestPanel final : public CPanel
{
private:
	CTestPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CTestPanel(const CTestPanel& rhs);
	virtual ~CTestPanel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components() ;
	virtual HRESULT Bind_ShaderResources() override;

private:
	HRESULT Ready_Button();

public:
	static CTestPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END

