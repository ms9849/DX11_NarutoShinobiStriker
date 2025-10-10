#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CGameInstance;
class CCollider;
NS_END

NS_BEGIN(Client)

class CEventObject abstract : public CGameObject
{
public:
	typedef struct tagEventColliderDesc {
		_float3 vPosition;
		_float3 vCenter;
		_float  fRadius;
		TRIGGER_TYPE eTriggerType = {};
	} EVENT_COLLIDER_DESC;

protected:
	CEventObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CEventObject(const CEventObject& Prototype);
	virtual ~CEventObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	/* 서로 다른 로직 구현 */
	virtual void OnCollision() = 0;

protected:
	/* 이벤트 객체는 기본적으로 콜라이더 & 사이즈 들고있게 */
	class CIcon*	m_pIcon = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	_float3			m_vColliderCenter = {};
	_float			m_fColliderRadius = {};
	TRIGGER_TYPE	m_eTriggerType = {};

private:
	HRESULT Ready_MissionIcon();

public:
	virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override;
};

NS_END
