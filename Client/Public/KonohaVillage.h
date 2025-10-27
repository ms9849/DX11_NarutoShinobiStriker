#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CMesh;
class CModel;
NS_END 

NS_BEGIN(Client)

class CKonohaVillage final : public CGameObject 
{
public:
	/* 나뭇잎 마을 네비게이션 구역 */
	enum class KONOHA_VILLAGE_SECTION { SECTION_1, SECTION_2, SECTION_3, SECTION_4, END };

private:
	CKonohaVillage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
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
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	_uint  m_iNumMeshes = {};
	CNavigation* m_NavigationComs[ENUM_CLASS(KONOHA_VILLAGE_SECTION::END)] = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CKonohaVillage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END