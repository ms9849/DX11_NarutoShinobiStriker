#pragma once

#include "Client_Defines.h"
#include "Enemy.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CNavigation;
class CCollider;
NS_END

/*
사용할 보스 패턴들 

1. 가까이와서 스핀 킥 -> 완료
2. 멀리서 7연 돌진 
3. 화염구 3연속 -> 완료
4. 사륜안 -> 이펙트 위주의 스킬이라 추후 수정 필요 
-> 이 4개는 확정. 혹시 필요하다면 플레이어 스킬 중에서 재탕할 것. 

5. 필요하면 슬라이딩 쿠나이 -> 완료
6. 평타 패턴 몇 개 -> 날아와서 차는건 완료
7. 목둔 박수 -> 완료

->
패턴 수정해야할 수도 있음.
평타 패턴도 일단 넣어놓아야 할거고..
*/

NS_BEGIN(Client)

class CBoss final : public CEnemy
{
public:
	/* 목둔 박수도 추가할까? */
	enum class BOSS_SKILL { FIREBALL, LIGHTING_RUSH, SHARINGAN, SPIN_KICK, WOODHAND, END };

private:
	CBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CBoss(const CBoss& rhs);
	virtual ~CBoss() = default;

public:
	_float		Get_AnimProgress();
	void		Set_AnimProgress(_float fProgress);
	void		Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate = 1.f, _bool IsBlend = true, _float fBlendRatio = 0.15f, _bool IsLoop = false);
	_bool		Play_Animation(_float fTimeDelta);
	void		Set_Collider_Active(const _wstring& strColliderTag, _bool bFlag);
	CCollider*  Get_Collider(const _wstring& strColliderTag);
	void		Set_Flying(_bool bFlag) { m_IsFlying = bFlag; }

public:
	virtual void OnCollision(COLLIDER_HANDLE_ID eHandleID) override;
	void Change_State(class CBossState* pNextState, _bool bBlend = true);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Start_Battle();
	_bool	Use_Skill(BOSS_SKILL eSkillList);
	void	Update_SkillCoolDown(_float fTimeDelta);

private:
	/* 추후 상태 추가해야함*/
	//class CWhiteJetsuState* m_pState = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CCollider* m_pHandAttackColliderCom = { nullptr };
	CCollider* m_pSpinKickColliderCom = { nullptr };
	CCollider* m_pRushColliderCom = { nullptr };

	class CBossState*	m_pState = { nullptr };
	class CIcon*		m_pIcon = { nullptr };
	class CBossHPPanel* m_pHPBar = { nullptr };
	_bool		m_IsPlayingDeadAnim = { false };
	/* 유니폼 초기화에서도 0 초기화 먹힘 */
	_float		m_SkillTimeAccs[ENUM_CLASS(BOSS_SKILL::END)] = { 0, };
	_float		m_SkillCoolDowns[ENUM_CLASS(BOSS_SKILL::END)] = { 0, };
	_bool		m_IsFlying = { false };
	_bool		m_IsActive = { true };

private:
	HRESULT Ready_Position();
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Ready_BossHPPanel();
	HRESULT Ready_BossIcon();
	void	Update_State(_float fTimeDelta);

public:
	static CBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
