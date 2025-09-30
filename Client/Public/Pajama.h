#pragma once

#include "Client_Defines.h"
#include "Enemy.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CNavigation;
class CCollider;
NS_END

NS_BEGIN(Client)

class CPajama final : public CEnemy
{
private:
	CPajama(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CPajama(const CPajama& rhs);
	virtual ~CPajama() = default;

public:
	_float		Get_AnimProgress();
	void		Set_AnimProgress(_float fProgress);
	void		Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate = 1.f, _bool IsBlend = true, _float fBlendRatio = 0.15f, _bool IsLoop = false);
	_bool		Play_Animation(_float fTimeDelta);
	void		Set_Collider_Active(const _wstring& strColliderTag, _bool bFlag);
	CCollider*	Get_Collider(const _wstring& strColliderTag);

public:
	virtual void OnCollision(COLLIDER_HANDLE_ID eHandleID) override;
	void Change_State(class CPajamaState* pNextState, _bool bBlend = true);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	/* 화둔 호화구 */
	/* 스킬을 쓸 수 있는 상태라면 True를 반환하고 상태에서 제어. */
	_bool Use_Skill();
	/* 작은 쿠나이 */
	_bool Use_Kunai();
	/* 슬라이딩 */
	_bool Use_Sliding();
	void  Update_SkillCoolDown(_float fTimeDelta);

private:
	/* 추후 상태 추가해야함*/
	//class CWhiteJetsuState* m_pState = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CCollider* m_pHandAttackColliderCom = { nullptr };

	class CPajamaState* m_pState = { nullptr };

	_float		m_fSkillTimeAcc = { 0.f };
	_float		m_fMaxSkillCoolDown = { 8.f };
	_float		m_fKunaiTimeAcc = { 0.f };
	_float		m_fMaxKunaiCoolDown = { 4.f };
	_float		m_fSlidingTimeAcc = { 0.f };
	_float		m_fMaxSlidingCoolDown = { 5.f };
	_bool		m_isPlayingDeadAnim = { false };

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	void	Update_State(_float fTimeDelta);

public:
	static CPajama* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
