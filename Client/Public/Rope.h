#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CRope final : public CGameObject
{
public:
	typedef struct tagRopeDesc : public GAMEOBJECT_DESC {
		_float3 vStartPos;
		_float3 vTargetPos;
		_float4x4 SocketMatrix;

	} ROPE_DESC;

private:
	CRope(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CRope(const CRope& rhs);
	virtual ~CRope() = default;

public:
	_bool IsArrive() {
		return m_IsArrive;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool		m_IsArrive = { false };
	_float3		m_vTargetPos = {};
	_float3		m_vTargetDir = {};

	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	_uint		m_iNumMeshes = {};
	_float4x4   m_WorldMatrix = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CRope* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};


NS_END

