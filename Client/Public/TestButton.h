#pragma once

#include "Client_Defines.h"
#include "Button.h"

NS_BEGIN(Client)

class CTestButton final : public CButton
{
private:
	CTestButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CTestButton(const CTestButton& rhs);
	virtual ~CTestButton() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT Ready_Components() override;

public:
	virtual _bool IsClicked() override;

public:
	static CTestButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
