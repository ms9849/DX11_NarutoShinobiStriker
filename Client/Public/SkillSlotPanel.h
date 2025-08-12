#pragma once

#include "Client_Defines.h"
#include "Skill_Table.h"
#include "Panel.h"

NS_BEGIN(Client)

class CSkillSlotPanel final : public CPanel
{
private:
	CSkillSlotPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CSkillSlotPanel(const CSkillSlotPanel& rhs);
	virtual ~CSkillSlotPanel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_SpecialSkillProgress(_float fProgress);
	void Set_MaxSpecialSkillProgress(_float fMaxProgress);
	void Change_Skill(_uint iSkillIdx, SKILL eSkill);

private:
	HRESULT Ready_SkillSlots();
	HRESULT Ready_ProgressBar();

public:
	static CSkillSlotPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END