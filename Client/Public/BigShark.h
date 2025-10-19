#pragma once

#include "Client_Defines.h"
#include "Skill.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Client)

/*
추후 록온 시스템 만들어지면 이 스킬은 가장 가까운 상대를 향해야 한다.
*/

class CBigShark final : public CSkill
{
public:
	typedef struct tagBigSharkDesc : public GAMEOBJECT_DESC {
		_float3 vPosition;
		_float3 vLook;
	} BIGSHARK_DESC;

private:
	CBigShark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CBigShark(const CBigShark& rhs);
	virtual ~CBigShark() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	/* 멤버 변수들 */
	class CEffectContainer* m_pEffectMain = { nullptr };
	_float			 m_fLifeTime = { 3.0f };
	_float			 m_fTimeAcc = { 0.f };
	_bool			 m_IsHit = { false };


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBigShark* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
