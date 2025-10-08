#pragma once

#include "Client_Defines.h"
#include "Skill_Table.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CCamera;
class CTransform;
class CGameObject;
class CCollider;
NS_END

NS_BEGIN(Client)

/*
클라이언트에 존재하는 유일한 싱글톤.

플레이어 관리, 
카메라 관리,
게임 내에서 다른 객체의 정보 필요할 때 세팅,

등등..

다양한 게임 내의 흐름을 컨트롤하는데 쓰이는 매니저가 될 것이다.

*/
/*
록온 시스템 -> 시야 내에 있고 (외적 결과 음수인가 양수인가로 판정) & 가장 가까운 몬스터가 록온되어야 할 것

플레이어는 공격할때 록온된 몬스터가 있다면 해당 몬스터를 향한다

몬스터의 트랜스폼은 게임 매니저가 들고있게?
*/

class CGameManager final : public CBase
{
	DECLARE_SINGLETON(CGameManager);

private:
	CGameManager();
	virtual ~CGameManager() = default;

public:
	LEVEL		Get_NextLevel();
	HRESULT		Set_NextLevelID(LEVEL eLevelID);

public:
	HRESULT		Initialize_GameManager();
	void		Release_GameManager();
	void		Clear();

#pragma region FLOW
	void  Set_Talking(_bool bFlag) { m_IsTalking = bFlag; }
	_bool IsTalking() { return m_IsTalking; }
#pragma endregion

#pragma region CAMERA
	_wstring    Get_CameraName();
	HRESULT		Add_TargetTransform(CTransform* pTransformCom);
	HRESULT		Add_Camera(LEVEL eLevelID, const _wstring& strCameraTag, CCamera* pCamera);
	HRESULT		Change_Camera(LEVEL eLevelID, const _wstring& strCameraTag, const _float4x4* pWorldMatrix = nullptr);
#pragma endregion

#pragma region COLLISION 
	/* 플레이어의 기본공격(검,주먹), 몬스터의 스킬 등, 콜라이더만 필요한 녀석들 */
	void Add_Collider_ToCollision(const _wstring& strColliderTag, COLLIDER_HANDLE_ID eHandleID, class CCollider* pCollider);
	void Add_Object_ToCollision(const _wstring& strObjectTag, CGameObject* pGameObject, CCollider* pCollider);

	void Update_Collision();
	/* Level의 업데이트에서 수행 */
	void Check_Collision(const _wstring strColliderTag, const _wstring strObjectTag, COLLISION_TYPE eColType);

#pragma endregion

#pragma region LockOn
	void		Update_LockOnManager(_float fTimeDelta);
	CTransform*	Calc_Target(_fvector vPosition);
#pragma endregion

#pragma region UI
	void Set_Dialog(class CDialogUI* pDialogUI);
	void Set_SkillSlotPanel(class CSkillSlotPanel* pSkillSlotPanel);
	void Set_AttackTypePanel(class CAttackTypePanel* pAttackTypePanel);
	void Set_ComboKoPanel(class CComboKOPanel* pComboKOPanel);
	void Set_MissionAlertPanel(class CMissionAlertPanel* pMissionAlertPanel);
	void Set_WinPanel(class CWinPanel* pWinPanel);
	void Set_CutScenePanel(class CCutScenePanel* pCutScenePanel);
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
#pragma endregion

#pragma region FLOW
	HRESULT	OnTrigger(TRIGGER_TYPE eTriggerType);
	TRIGGER_TYPE Get_CurrentTrigger();
#pragma endregion

#pragma region PLAYER
	class CPlayer* Get_PlayerPtr();
	HRESULT	Set_PlayerPtr(class CPlayer* pPlayer);
	vector<pair<_wstring, _wstring>>& Get_Outfits(SELECT_TYPE eType);
	void Add_Outfit(SELECT_TYPE eType, const _wstring& strOutfitTag, const _wstring& strModelTag);
	void Set_PlayerModelInfo(SELECT_TYPE eType, _uint iModelNum);
	_wstring Get_PlayerModelInfo(SELECT_TYPE eType);
	_bool IsOneCloth();

#pragma endregion

private:
	CGameInstance* m_pGameInstance = { nullptr };

	class CCamera_Manager* m_pCamera_Manager = { nullptr };
	class CCollision_Manager* m_pCollision_Manager = { nullptr };
	class CLockOn_Manager* m_pLockOn_Manager = { nullptr };
	class CUI_Manager* m_pUI_Manager = { nullptr };
	class CTrigger_Manager* m_pTrigger_Manager = { nullptr };
	class CPlayer_Manager* m_pPlayer_Manager = { nullptr };

	/* 게임 매니저 단에서 직접 관리할 변수들 */
	_bool			m_IsTalking = { false };
	LEVEL			m_eNextLevel = {};

public:
	virtual void Free() override;
};

NS_END
