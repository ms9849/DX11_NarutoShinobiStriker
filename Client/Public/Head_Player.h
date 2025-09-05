#pragma once

#include "Client_Defines.h"
#include "Parts_Player.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CHead_Player final : public CParts_Player
{
public:
	typedef struct tagHead_Player_Desc : public CPartObject::PARTOBJECT_DESC
	{
		const _uint* pParentState = { nullptr };
	} HEAD_PLAYER_DESC;

private:
	CHead_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CHead_Player(const CHead_Player& Prototype);
	virtual ~CHead_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CHead_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END