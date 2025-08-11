#pragma once

#include "Client_Defines.h"
#include "Skill_Table.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Client)

/*
1. 캐릭터단에 스킬슬롯 둠?
-> 두는것도 괜찮아보이긴 하는데..
*/

class CCharacter abstract : public CGameObject
{
public:
	enum class ATTACK_TYPE {
		MELEE, NINJUTSU, DEFENSIVE, END
	};

protected:
	CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Change_AttackType();

	const SKILL_INFO& Get_Skill_Info(const SKILL& eSkill) {
		auto iter = m_Skills.find(eSkill);

		return iter->second;
	}

protected:
	class CGameManager* m_pGameManager = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	
protected:
	// 스킬에 따라 1,2,3번 스킬이 변경됨
	// 스킬은 곧 모션(애니메이션)이니까?
	// 지금 스킬 구조를 짜는건 좀 이상하긴 한데..
	// 나중에 애니메이션 뽑아내서 그거 따로 저장해두고 세팅해두는 형식으로 가야할 듯?
	ATTACK_TYPE m_eCurAttackType = { ATTACK_TYPE::MELEE };
	ATTACK_TYPE m_ePreAttackType = { ATTACK_TYPE::END };

	/* 활성화된 스킬들의 ENUM CLASS를 들고 있게 한다. */
	/* 0~2번은 일반 스킬, 3번은 필살기 */
	SKILL m_ActivatedSkills[4];
	map<SKILL, SKILL_INFO> m_Skills;
	/*
	스킬 INFO는 Map으로 따로 관리.
	m_ActivatedSkills를 순회하면서 현재 활성화된 스킬들의 정보만
	업데이트해주고, 정보들을 가져올 수 있게 해줌.
	*/

protected:
	HRESULT Bind_ShaderResources();

public:
	virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override;
};

NS_END
