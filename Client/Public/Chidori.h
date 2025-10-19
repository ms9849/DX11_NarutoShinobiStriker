#pragma once

#include "Client_Defines.h"
#include "Skill.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Client)

class CChidori final : public CSkill
{
public:
	typedef struct tagChidoriDesc {
		const _float4x4* pSocketMatrix;
	} CHIDORI_DESC;
private:
	CChidori(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CChidori(const CChidori& rhs);
	virtual ~CChidori() = default;

public:
	_bool IsColliderActive();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	/* ¸â¹ö º¯¼öµé */
	class CEffectContainer* m_pEffectMain = { nullptr };
	const _float4x4* m_pSocketMatrix;
	_float4x4		 m_CombinedWorldMatrix = {};
	_float			 m_fLifeTime = { 1.2f };
	_float			 m_fTimeAcc = { 0.f };
	_bool			 m_IsHit = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CChidori* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
