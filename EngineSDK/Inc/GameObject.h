#pragma once

#include "Transform.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc : public CTransform::TRANSFORM_DESC {

	} GAMEOBJECT_DESC;
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iObjectID);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Set_Desc(void* pDesc);

public:
	_bool IsDead() const { return m_IsDead; }
	void Set_Dead(_bool bFlag) { m_IsDead = bFlag; }

	_uint Get_ObjectID() const { return m_iObjectID; }
	class CTransform* Get_Transform() { return m_pTransformCom; }
	_bool Get_Pickable() { return m_IsPickable; }
	void Set_Pickable(_bool bFlag) { m_IsPickable = bFlag; }

	virtual void Set_Gravity(_bool bFlag, _float fDist = 0) {}

public:
	_float Get_CamDistance() const;
	class CComponent* Find_Component(const _wstring& strComponentTag);

protected:
	_bool m_IsGravity = { false };
	_uint m_iObjectID = {};
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	_bool						m_IsDead = { false };
	_bool						m_IsPickable = { true };
	/* Transform은 GameObject에서 들고 있게끔 한다. */
	class CTransform* m_pTransformCom = { nullptr };
	map<const _wstring, class CComponent*>		m_Components;

protected:

	/* 컴포넌트 이름을 따로 입력받지 않는, 엔진 단에서 키값을 제공해주는 컴포넌트 */
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag,
		CComponent** ppOut, void* pArg = nullptr);

	/* 컴포넌트 이름을 입력받는 함수. 키값을 따로 지정해주어야 한다. */
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, 
		const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END