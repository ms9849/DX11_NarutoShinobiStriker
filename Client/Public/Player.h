#pragma once
#include "Character.h"

 
// 플레이어에게 필요한 정보
// 1. 스킬 타입 
// 2. 스킬 쿨타임
// 3. 필살기 게이지

// 이걸 굳이 플레이어의 정보 저장소를 따로 만들어서 빼야하나?
// 트랜스폼 같은건 이미 묶여있는데..
// SkillType 클래스 만들어서 구분 가능하게 하고
// Activated Skill Type 반환하게 하고.. (쿨타임 / 게이지도 그렇게 관리 )
// Getter Setter는 어쩔수 없이 열어야함...

/*
CContainer ->
CCharacter ->
CPlayer 의 계층구조
*/

NS_BEGIN(Client)

class CPlayer final : public CCharacter
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	class CTransform* Get_PlayerTransformPtr() {
		return m_pTransformCom;
	}

public:
	void	Set_SkillSlotPanel(class CSkillSlotPanel* pPanel);
	void	Set_AttackTypePanel(class CAttackTypePanel* pPanel);
	void	Set_ComboKOPanel(class CComboKOPanel* pPanel);
	void	Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate = 1.f, _bool IsBlend = true, _float fBlendRatio = 0.15f);

public:
	void	Clear_State();
	void	Update_State(_float fTimeDelta);
	_bool	Play_Animation(_float fTimeDelta);
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CGameManager* m_pGameManager = { nullptr };
	class CSkillSlotPanel* m_pSkillSlotPanel = { nullptr };
	class CAttackTypePanel* m_pAttackTypePanel = { nullptr };
	class CComboKOPanel* m_pComboKOPanel = { nullptr };

	class CPlayerState* m_pState = { nullptr };

	_uint m_iComboCount = { 0 };
	_uint  m_iMaxComboCount = { 99 };

	_float m_fComboTimeAcc = { 0 };
	_bool m_bEnemyHit = { false };
	_bool m_bEnemyKO = { false };

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

private:
	void Key_Input(_float fTimeDelta);
	void ComboKO_System(_float fTimeDelta);
	void Change_Skills();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END


/*
//그럼 내일 할 일은...
// 0. 플레이어 작성해서 모델 붙이기 -> 완
// 1. 스킬 구조 플레이어 내부에 작성 -> AttackType은 열거형으로 작성. 스킬 두개 (나선환 나선수리검) 작성해서 일단 추가해놓고..
// 2. 퀘스트 작성 및 UI 묶기
*/