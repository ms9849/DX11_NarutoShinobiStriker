#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CGameInstance;
class CCollider;
NS_END

NS_BEGIN(Client)

class CTriggerBox abstract : public CGameObject
{
public:
	typedef struct tagTriggerBoxDesc {
		_float3 vPosition;
		_float3 vCenter;
		_float  fRadius;
	} TRIGGER_BOX_DESC;

protected:
	CTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CTriggerBox(const CTriggerBox& Prototype);
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
	virtual void OnCollision() = 0;

protected:
	/* 트리거박스는 기본적으로 콜라이더 & 사이즈 들고있게 */
	CCollider* m_pColliderCom = { nullptr };
	_float3    m_vColliderCenter = {};
	_float	   m_fColliderRadius = {};

public:
	virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override;
};

NS_END
