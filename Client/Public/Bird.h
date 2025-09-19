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

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

	_uint m_iNumMeshes = {};

	/* 제츠 전용 상태 */
	/*
	플레이어랑은 다르게 어딘가에서 참조되고 있지 않으니까..
	따로 Release 해줄 메서드를 만들지 않아도 된다.
	*/
	_bool m_isPlayingDeadAnim = { false };

	class CBirdState*	m_pState = { nullptr };
	CNavigation*		m_pNavigationCom = { nullptr };
	CCollider*			m_pColliderCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void	Update_State(_float fTimeDelta);
public:
	static CBird* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
