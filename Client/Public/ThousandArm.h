#pragma once

#include "Client_Defines.h"
#include "Skill.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

/* 
∫Œ√≥ ∞¥√º
*/
class CThousandArm final : public CSkill
{
private:
	CThousandArm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CThousandArm(const CThousandArm& rhs);
	virtual ~CThousandArm() = default;

public:
	void Set_Visible(_bool bFlag) { m_IsVisible = bFlag; }
	void Set_Active(_bool bFlag) { m_IsActive = bFlag; }
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float   m_fWoodArmCoolDownTimeAcc = { 0.f };
	_float   m_fWoodArmCoolDown = { 1.5f };
	_float	 m_fLifeTimeAcc = { 0.f };
	_uint	 m_iNumMeshes = { 0 };
	_bool    m_IsVisible = { false }; 
	_bool    m_IsActive = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CThousandArm* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

NS_END