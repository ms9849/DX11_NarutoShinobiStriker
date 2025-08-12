#pragma once

#include "Client_Defines.h"
#include "Skill_Table.h"
#include "UIObject.h"

NS_BEGIN(Client)

class CSkillSlotUI : public CUIObject
{
public:
	typedef struct tagSkillSlotDesc : public UIOBJECT_DESC {
		_uint iSkillNum;
	} SKILLSLOT_DESC;

private:
	CSkillSlotUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CSkillSlotUI(const CSkillSlotUI& rhs);
	virtual ~CSkillSlotUI() = default;

public:
	void Change_Skill(SKILL eSkill);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CGameManager* m_pGameManager = { nullptr };
	CTexture*			m_pTextureCom_Skill = { nullptr };
	_uint				m_iTextureIdx_Skill = {};
	_uint				m_iSkillNum = {};

private:
	HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CSkillSlotUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;;
	virtual void Free() override;
};

NS_END
