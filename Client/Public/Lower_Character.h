#pragma once

#include "Client_Defines.h"
#include "Parts_Character.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CLower_Character final : public CParts_Character
{
public:
	typedef struct tagLower_Player_Desc : public CHARACTER_PART_DESC
	{
	}LOWER_PLAYER_DESC;

private:
	CLower_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CLower_Character(const CLower_Character& Prototype);
	virtual ~CLower_Character() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	class CTrail* m_pFootTrail[2] = { nullptr }; 
	_float m_fTimeAcc = { 0.f };
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLower_Character* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END