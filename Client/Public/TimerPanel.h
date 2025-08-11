#pragma once

#include "Client_Defines.h"
#include "Panel.h"

NS_BEGIN(Client)

class CTimerPanel final : public CPanel
{
private:
	CTimerPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CTimerPanel(const CTimerPanel& rhs);
	virtual ~CTimerPanel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Add_LeftTime(_uint iSec) {
		m_iLeftTime += iSec;
	}

private:
	_float m_fTimeAcc = {};
	_uint m_iLeftTime = {};

private:
	HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources() override;

private:
	HRESULT Ready_Decimals();
	void Calc_Timer();

public:
	static CTimerPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
