#pragma once

#include "Client_Defines.h"
#include "Skill.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

/* */
NS_BEGIN(Client)

class CRasenShuriken final : public CSkill
{
public:
	typedef struct tagRasenShurikenDesc : public GAMEOBJECT_DESC {
		_float3 vDir;
		const _float4x4* pSocketMatrix;
	} RASENSHURIKEN_DESC;
private:
	CRasenShuriken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CRasenShuriken(const CRasenShuriken& rhs);
	virtual ~CRasenShuriken() = default;

public:
	void Throw();
	void Set_Scale(_float fX, _float fY, _float fZ) {
		m_pTransformCom->Set_Scale(fX, fY, fZ);
	}
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	/* ¸â¹ö º¯¼öµé */
	class CEffectContainer* m_pEffectShuriken = { nullptr };
	class CEffectContainer* m_pEffectExplosion = { nullptr };

	const _float4x4* m_pSocketMatrix;
	_float4x4		 m_CombinedWorldMatrix = {};

	_bool			 m_isInHand = { true };
	_bool			 m_isThrowing = { false };

	_float			 m_fLifeTime = { 2.0 };
	_float			 m_fTimeAcc = { 0.f };
	_bool			 m_IsHit = { false };
	_float3			 m_vDirection = { };
	_float			 m_fSpeed = { 30.f };

	_float			 m_fAttackCoolDown = { 0.f };
	_float			 m_fAttackMaxCoolDown = { 0.15f };
	_bool			 m_isFinal = { false };

	_float			 m_fStartScale = { 0.1f };
	_float			 m_fDeltaScaleTimeAcc = { 0.f };
	_float			 m_fDeltaScaleTime = { 2.f };
	_bool			 m_IsExplosion = { false };
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CRasenShuriken* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
