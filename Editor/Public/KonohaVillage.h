#pragma once

#include "Editor_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END 

NS_BEGIN(Editor)

class CKonohaVillage final : public CGameObject 
{
private:
	CKonohaVillage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	CKonohaVillage(const CKonohaVillage& rhs);
	virtual ~CKonohaVillage() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	HRESULT Render();

private:
	class CGameManager* m_pGameManager = { nullptr };

	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	_uint  m_iNumMeshes = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CKonohaVillage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END