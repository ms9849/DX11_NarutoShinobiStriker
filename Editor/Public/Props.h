#pragma once

#include "Editor_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Editor)

/* 
고유한 아이디를 가지긴 하지만 내부에서 상태가 달라짐..
이걸 세팅해주기 위해선 로딩할 떄 다른 방식이 필요할 것이다.
*/

/*
프롭은.. 화분이랑 꽃들 뿐이지?
*/

class CProps : public CGameObject
{
public:
	typedef struct tagPropDesc : public GAMEOBJECT_DESC {
		_uint iMeshIdx;
		_uint iShaderPassIdx;
	} PROP_DESC;

private:
	CProps(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	CProps(const CProps& rhs);
	virtual ~CProps() = default;

public:
	_uint Get_MeshIdx() {
		return m_iMeshIdx;
	}

	_uint Get_ShaderPassIdx() {
		return m_iShaderPassIdx;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };

	_uint		m_iMeshIdx = {};
	_uint		m_iShaderPassIdx = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CProps* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
