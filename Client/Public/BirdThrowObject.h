#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
NS_END 

NS_BEGIN(Client)

class CBirdThrowObject : public CGameObject
{
public:
	typedef struct tagBirdThrowObject : public GAMEOBJECT_DESC {
		_float3 vPosition;
		_float3 vDirection;

	} BIRD_THROW_DESC;

private:
	CBirdThrowObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CBirdThrowObject(const CBirdThrowObject& rhs);
	virtual ~CBirdThrowObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CGameManager* m_pGameManager = { nullptr };
	CCollider*	m_pColliderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	_uint		m_iNumMeshes = {};
	_float3		m_vDirection = {};
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBirdThrowObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CBirdThrowObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
