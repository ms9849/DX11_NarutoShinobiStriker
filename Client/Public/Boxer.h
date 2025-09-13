#pragma once

#include "Client_Defines.h"
#include "Enemy.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CNavigation;
NS_END

NS_BEGIN(Client)

class CBoxer final : public CEnemy
{
private:
	CBoxer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CBoxer(const CBoxer& rhs);
	virtual ~CBoxer() = default;

public:
	_float	Get_AnimProgress();
	void	Set_AnimProgress(_float fProgress);
	void	Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate = 1.f, _bool IsBlend = true, _float fBlendRatio = 0.15f, _bool IsLoop = false);
	_bool	Play_Animation(_float fTimeDelta);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	/* 나뭇잎 선풍 */
	/* 스킬을 쓸 수 있는 상태라면 True를 반환하고 상태에서 제어. */
	_bool Use_Skill();
	void  Update_SkillCoolDown(_float fTimeDelta);
private:
	/* 추후 상태 추가해야함*/
	//class CWhiteJetsuState* m_pState = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	class CBoxerState* m_pState = { nullptr };

	_float		m_fSkillTimeAcc = { 0.f };
	_float		m_fMaxSkillCoolDown = { 10.f };

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	void	Update_State(_float fTimeDelta);

public:
	static CBoxer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
