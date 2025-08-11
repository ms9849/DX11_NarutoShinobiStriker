#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Client)

class CSkillSlot : public CUIObject
{
private:
	CSkillSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CSkillSlot(const CSkillSlot& rhs);
	virtual ~CSkillSlot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CGameManager* m_pGameManager = { nullptr };
private:
	HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CSkillSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;;
	virtual void Free() override;
};

NS_END
