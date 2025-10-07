#pragma once

#include "Client_Defines.h"
#include "Skill_Table.h"
#include "Base.h"

NS_BEGIN(Engine)
class CTransform;
class CGameInstance;
NS_END

NS_BEGIN(Client)

/*
렌더러처럼 매 프레임 트랜스폼 넣었다가 빼줄 것.
-> 정렬은 언제?
-> 할 필요 없나? 가장 가까운 트랜스폼만 가져다가 쓰면 됨.
-> 입력받은걸 삭제하는 타이밍은 언제?

가장 가까운 몬스터의 위치를 찾아야 한다. ->
매프레임 Find 해서 가장 가까운 몬스터의 위치를 찾는 것도 괜찮아보임

1. 기본적으로 가장 가까운 몬스터를 바라본다
2. 3초 이내에 공격한 적이 있으면 해당 적을 바라본다.
3. 2초 동안 시야 내에 없다면 (절두체컬링 이용) 새로운 적을 찾는다 (아직 구현은 불가능함)
*/

/*
이녀석의 업데이트는 플레이어에서 해줄 것.
락 온과 관련된 로직은 플레이어 + 카메라에서만 활용되니까 큰 문제는 없을 것
*/

class CUI_Manager final : public CBase
{
private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	HRESULT Initialize();

public:
	void Set_Dialog(class CDialogUI* pDialogUI);
	void Set_SkillSlotPanel(class CSkillSlotPanel* pSkillSlotPanel);
	void Set_AttackTypePanel(class CAttackTypePanel* pAttackTypePanel);
	void Set_ComboKoPanel(class CComboKOPanel* pComboKOPanel);
	void Set_MissionAlertPanel(class CMissionAlertPanel* pMissionAlertPanel);
	void Set_WinPanel(class CWinPanel* pWinPanel);
	void Set_CutScenePanel(class CCutScenePanel* pCutScenePanel);

public:
	void Set_Dialog_Text(const _wstring& strDialogText);
	void Set_Dialog_Visible(_bool bFlag);

	void Active_Combo();
	void Active_KO();

	void Change_Skill(_uint iIdx, SKILL eSkill);
	void Set_SkillSlot_Visible(_bool bFlag);

	void Change_AttackType(ATTACK_TYPE eAttackType);
	void Set_HpProgress(_float fProgress);
	void Set_MaxHpProgress(_float fMaxProgress);
	void Set_AttackType_Visible(_bool bFlag);

	void AlertPanel_Start_FadeIn(const _wstring& strMissionText);
	void WinPanel_Start_FadeIn();
	
	void Set_CutScene_Visible(_bool bFlag);

private:
	CGameInstance* m_pGameInstance = { nullptr };

	class CDialogUI* m_pDialogUI = { nullptr };
	class CSkillSlotPanel* m_pSkillSlotPanel = { nullptr };
	class CAttackTypePanel* m_pAttackTypePanel = { nullptr };
	class CComboKOPanel* m_pComboKOPanel = { nullptr };
	class CMissionAlertPanel* m_pMissionAlertPanel = { nullptr };
	class CWinPanel* m_pWinPanel = { nullptr };
	class CCutScenePanel* m_pCutScenePanel = { nullptr };
public:
	static CUI_Manager* Create();
	virtual void Free() override;
};

NS_END