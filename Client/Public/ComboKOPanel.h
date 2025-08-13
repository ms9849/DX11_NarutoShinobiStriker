#pragma once

#include "Client_Defines.h"
#include "Panel.h"

/* 
콤보는 최대 99까지. 
1 콤보 -> 큰 HIT로 땜빵
2 콤보 이상 -> Decimal UI 가져다 써야할 듯

KO -> 별개로 적 처치시에 띄워줄 거니까.. UI 별개로 필요함. (KO랑 콤보랑은 다름)

Hit, Hits는 ComboPanel에서 출력.
숫자는 Decimal.

KO는 KO UI 에서.


*/

NS_BEGIN(Client)

class CComboKOPanel final : public CPanel
{
private:
	CComboKOPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CComboKOPanel(const CComboKOPanel& rhs);
	virtual ~CComboKOPanel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:

private:
	HRESULT Ready_Components();
	HRESULT Ready_Deicmals();
	HRESULT Ready_KO();
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CComboKOPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END