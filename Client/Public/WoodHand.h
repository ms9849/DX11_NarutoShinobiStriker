#pragma once

//¸ñµÐ¹Ú¼ö Â¦Â¦~

#include "Client_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CCollider;
NS_END 

NS_BEGIN(Client)

class CWoodHand final : public CContainerObject
{
public:
	typedef struct tagWoodHandDesc : public GAMEOBJECT_DESC
	{
		_float3 vTargetPos;
		_float3 vJetsuPos;
	} WOODHAND_DESC;

private:
	CWoodHand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CWoodHand(const CWoodHand& rhs);
	virtual ~CWoodHand() = default;

public:
	void Attack();
	_float Get_AnimProgress();
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CGameManager* m_pGameManager = { nullptr };
	CCollider*  m_pColliderCom = { nullptr };
	_float3		m_vTargetPos = { };
	_bool		m_IsMoveFinished = { false };
	_bool 		m_IsAttackFinished = { false };
	_float		m_fTimeAcc = { 0.f };
	_bool		m_isAttackOn = { false };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Childs();

public:
	static CWoodHand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
