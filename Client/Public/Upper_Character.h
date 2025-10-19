#pragma once

#include "Client_Defines.h"
#include "Parts_Character.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CUpper_Character final : public CParts_Character
{
public:
	typedef struct tagUpper_Player_Desc : public CHARACTER_PART_DESC
	{
	}UPPER_PLAYER_DESC;

private:
	CUpper_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CUpper_Character(const CUpper_Character& Prototype);
	virtual ~CUpper_Character() = default;

public:
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CUpper_Character* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END