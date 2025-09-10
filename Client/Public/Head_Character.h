#pragma once

#include "Client_Defines.h"
#include "Parts_Character.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CHead_Character final : public CParts_Character
{
public:
	typedef struct tagHead_Player_Desc : public CHARACTER_PART_DESC
	{
	} HEAD_PLAYER_DESC;

private:
	CHead_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CHead_Character(const CHead_Character& Prototype);
	virtual ~CHead_Character() = default;

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
	static CHead_Character* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END