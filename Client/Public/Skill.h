#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
/* 스킬 테이블 헤더는 스킬마다 다 필요할테니까 */
#include "Skill_Table.h"

NS_BEGIN(Engine)
class CModel;
class CTexture;
class CShader;
class CCollider;
NS_END 

NS_BEGIN(Client)

class CSkill abstract: public CGameObject
{
protected:
	CSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CSkill(const CSkill& rhs);
	virtual ~CSkill() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CModel*		m_pModelCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	CCollider*	m_pColliderCom = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override = 0;
};

NS_END
