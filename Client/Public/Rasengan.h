#pragma once

#include "Client_Defines.h"
#include "Skill.h"

NS_BEGIN(Engine)
class CCollider;
NS_END 

NS_BEGIN(Client)

class CRasengan final : public CSkill
{
public:
	typedef struct tagRasenganDesc {
		const _float4x4* pSocketMatrix;
	} RASENGAN_DESC;
private:
	CRasengan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CRasengan(const CRasengan& rhs);
	virtual ~CRasengan() = default;

public:
	_bool IsColliderActive();
	void Toggle_Effect();
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CParticleObject*  m_pParticleMain = { nullptr };
	class CEffectContainer* m_pEffectCharge = { nullptr };
	class CEffectContainer* m_pEffectRun = { nullptr };
	const _float4x4* m_pSocketMatrix;
	_float4x4		 m_CombinedWorldMatrix = {};
	_float			 m_fLifeTime = { 1.2f };
	_float			 m_fTimeAcc = { 0.f }; 
	_bool			 m_IsHit = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CRasengan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
