#pragma once

#include "Client_Defines.h"
#include "Enemy.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
NS_END 

NS_BEGIN(Client)

class CWhiteJetsu final : public CEnemy
{
private:
	CWhiteJetsu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CWhiteJetsu(const CWhiteJetsu& rhs);
	virtual ~CWhiteJetsu() = default;

public:
	virtual _wstring Get_CurrentAnim() override;
	virtual _float	Get_AnimProgress();
	virtual void	Set_AnimProgress(_float fProgress);
	virtual void	Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate = 1.f, _bool IsBlend = true, _float fBlendRatio = 0.15f, _bool IsLoop = false);
	virtual _bool	Play_Animation(_float fTimeDelta);

public:
	virtual void OnCollision(COLLIDER_HANDLE_ID eHandleID) override;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	/* 목둔 박수 */
	/* 스킬을 쓸 수 있는 상태라면 True를 반환하고 상태에서 제어. */
	_bool Use_Skill();
	void  Update_SkillCoolDown(_float fTimeDelta);

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	_uint m_iNumMeshes = {};

	/* 제츠 전용 상태 */
	/* 
	플레이어랑은 다르게 어딘가에서 참조되고 있지 않으니까.. 
	따로 Release 해줄 메서드를 만들지 않아도 된다.
	*/
	class CWhiteJetsuState* m_pState = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };

	_float m_fSkillTimeAcc = { 0.f }; 
	_float m_fMaxSkillCoolDown = { 10.f };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void	Update_State(_float fTimeDelta);
public:
	static CWhiteJetsu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
