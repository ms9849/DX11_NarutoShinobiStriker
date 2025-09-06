#pragma once

#include "Client_Defines.h"
#include "Parts_Player.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CUpper_Player final : public CParts_Player
{
public:
	typedef struct tagUpper_Player_Desc : public CPartObject::PARTOBJECT_DESC
	{
	}UPPER_PLAYER_DESC;

private:
	CUpper_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CUpper_Player(const CUpper_Player& Prototype);
	virtual ~CUpper_Player() = default;

public:
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;

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
	static CUpper_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END