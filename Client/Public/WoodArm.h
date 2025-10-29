#pragma once

#include "Client_Defines.h"
#include "Skill.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

/*
³¯¾Æ°¥ ÆÈ °´Ã¼ 
*/
class CWoodArm final : public CSkill
{
public:
	typedef struct tagWoodArmDesc : public CGameObject::GAMEOBJECT_DESC {
		_float4 vLookPos;
		_float4 vStartPos;
	}WOODARM_DESC;
private:
	CWoodArm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CWoodArm(const CWoodArm& rhs);
	virtual ~CWoodArm() = default;

public:
	void Set_Visible(_bool bFlag) { m_IsVisible = bFlag; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint	 m_iNumMeshes = { 0 };
	_bool    m_IsVisible = { false };
	_bool    m_IsShaked = { false };
	_float4  m_vTargetPos = {};
	_float   m_fDeadTimeAcc = { 0.f };
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CWoodArm* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

NS_END