#pragma once

#include "Client_Defines.h"
#include "Enemy.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
NS_END

NS_BEGIN(Client)

class CBird final : public CEnemy
{
private:
	CBird(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CBird(const CBird& rhs);
	virtual ~CBird() = default;

public:
	virtual _wstring Get_CurrentAnim() override;
	virtual _float	Get_AnimProgress();
	virtual void	Set_AnimProgress(_float fProgress);
	virtual void	Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate = 1.f, _bool IsBlend = true, _float fBlendRatio = 0.15f, _bool IsLoop = false);
	virtual _bool	Play_Animation(_float fTimeDelta);

public:
	virtual void OnCollision(COLLIDER_HANDLE_ID eHandleID) override;
	void Change_State(class CBirdState* pNextState, _bool bBlend = true);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Calc_HitEffectTime(_float fTimeDelta);
	void Set_HitEffect(_float fEffectTime, _float fIntensity = 1.f);
	_bool Use_Skill();
	void  Update_SkillCoolDown(_float fTimeDelta);

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

	_uint m_iNumMeshes = {};

	_float m_fSkillTimeAcc = { 0.f };
	_float m_fMaxSkillCoolDown = { 4.f };

	_bool m_IsPlayingDeadAnim = { false };

	_uint  m_iShaderPassIdx = { 0 };

	_float m_fEffectTime = { 0.f };
	_float m_fEffectTimeAcc = { 0.f };
	_float m_fIntensity = { 0.f };

	class CBirdState*	m_pState = { nullptr };
	CNavigation*		m_pNavigationCom = { nullptr };
	CCollider*			m_pColliderCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Position();
	HRESULT Bind_ShaderResources();
	void	Update_State(_float fTimeDelta);
public:
	static CBird* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
