#pragma once

#include "Client_Defines.h"
#include "EventObject.h"

NS_BEGIN(Client)

class CTriggerBox final : public CEventObject
{
private:
	CTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CTriggerBox();
	virtual ~CTriggerBox() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	/* 트리거마다 다른 이벤트 구현 */
	virtual void OnCollision() override;

private:
	class CGameManager* m_pGameManager = { nullptr };
	class CEffectContainer* m_pEffectMain = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CTriggerBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
