#pragma once

#include "Client_Defines.h"
#include "Skill.h"

NS_BEGIN(Engine)
class CCollider;
class CVIBuffer_Rect;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Client)

class CKamui final : public CSkill
{
public:
	typedef struct tagKamuiDesc {
		_float3 vPosition;
		_float3 vLook;
	} KAMUI_DESC;

private:
	CKamui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CKamui(const CKamui& rhs);
	virtual ~CKamui() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	/* ¸â¹ö º¯¼öµé */
	class CEffectContainer*		m_pEffectContainer = { nullptr };
	class CVIBuffer_Rect*		m_pVIBufferCom = { nullptr }; 
	_float						m_fLifeTime = { 2.0f };
	_float						m_fTimeAcc = { 0.f };
	_bool						m_isFinal = { false };
	_float						m_fAttackCoolDown = { 0.f };
	_float						m_fAttackMaxCoolDown = { 0.15f };
	_float4x4					m_ColliderWorldMatrix = {};
	_float						m_fParticleTimeAcc = { 0.f };
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CKamui* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
