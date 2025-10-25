#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END 

NS_BEGIN(Client)

class CSkyMesh final : public CGameObject
{
public:
	typedef struct tagSkyMeshDesc : public GAMEOBJECT_DESC {
		_bool IsNight = { false };

	} SKYMESH_DESC;
private:
	CSkyMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CSkyMesh(const CSkyMesh& rhs);
	virtual ~CSkyMesh() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel*  m_pModelCom = { nullptr };
	_uint	 m_iNumMeshes = {};
	_uint    m_iShaderPassIdx = { 1 };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSkyMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

NS_END