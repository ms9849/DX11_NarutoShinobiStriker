#pragma once

#include "Client_Defines.h"
#include "Parts_Character.h"

NS_BEGIN(Engine)
class CGameInstance;
class CModel;
class CShader;
class CTexture;
class CCollider;
NS_END

NS_BEGIN(Client)

class CWeapon_Character final : public CParts_Character
{
public:
	enum class WEAPON_TYPE { SWORD, GLOVE };
	typedef struct tagWeapon_Player_Desc : public CHARACTER_PART_DESC
	{
		CParts_Character* pUpper_Player = { nullptr };
		const _float4x4* pAttachMatrix = { nullptr };
		const _float4x4* pHandMatrix = { nullptr };
		WEAPON_TYPE      eType = {};

	} WEAPON_PLAYER_DESC;

private:
	CWeapon_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CWeapon_Character(const CWeapon_Character& Prototype);
	virtual ~CWeapon_Character() = default;

public:
	CCollider* Get_Collider() { return m_pColliderCom;  }
	virtual void	Set_Collider_Active(_bool bFlag);
	virtual void	Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate = 1.f, _bool IsBlend = true, _float fBlendRatio = 0.15f, _bool IsLoop = false) override;
	virtual _bool	Play_Animation(_float fTimeDelta) override;
	virtual void	Set_AnimProgress(_float fProgress) override;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	_float		m_fTimeAcc = { 0.f };
	_bool		m_IsCreateTrail = { false };
	_string		m_strCurrentAnimName = {};
	const _float4x4* m_pAttachMatrix = { nullptr };
	const _float4x4* m_pHandMatrix = { nullptr };
	CParts_Character* m_pUpper_Player = { nullptr };
	/* 등 본에 붙어 있는가 */
	_bool m_IsAttached = { true };
	WEAPON_TYPE m_eType = {};

	CCollider* m_pColliderCom = { nullptr };

	class CTrail* m_pSwordTrail = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CWeapon_Character* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END