#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
NS_END 

/* 쿠나이 방향 계산 해야함 */
NS_BEGIN(Client)

class CKunai : public CGameObject
{
public:
	typedef struct tagBirdThrowObject : public GAMEOBJECT_DESC {
		_float3 vPosition;
		_float3 vDirection;

	} KUNAI_DESC;

private:
	CKunai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CKunai(const CKunai& rhs);
	virtual ~CKunai() = default;

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
	_float		m_fTimeAcc = { 0.f };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CKunai* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CKunai* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
