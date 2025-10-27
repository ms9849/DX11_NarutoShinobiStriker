#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CEnemy abstract : public CContainerObject
{
protected:
	CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CEnemy(const CEnemy& rhs);
	virtual ~CEnemy() = default;

public:
	virtual _wstring	Get_CurrentAnim();
	virtual void	Set_Invincible(_float fInvincibleTime);
	virtual _float	Get_AnimProgress();
	virtual void	Set_AnimProgress(_float fProgress);
	virtual void	Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate = 1.f, _bool IsBlend = true, _float fBlendRatio = 0.15f, _bool IsLoop = false);
	virtual _bool	Play_Animation(_float fTimeDelta);

public:
	void Fade_Particle();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollision(COLLIDER_HANDLE_ID eHandleID);

protected:
	HRESULT Ready_HPBar();
	HRESULT Update_FootTrail(_float fTimeDelta);
	HRESULT LateUpdate_FootTrail(_float fTimeDelta);
protected:
	_float m_fFootTrailTimeAcc = { 0.f };
	class CTrail* m_pFootTrail[2] = { nullptr };
	_uint  m_iShaderPassIdx = { 0 };
	_bool  m_IsInvincible = { false };
	_float m_fInvincibleTime = { 0.f };
	_float m_fCurrentHP = { 100 };
	_float m_fMaxHP = { 200 };
	class CEnemy_HPBar* m_pHPBar = { nullptr };
	class CGameManager* m_pGameManager = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override = 0;
};

NS_END
