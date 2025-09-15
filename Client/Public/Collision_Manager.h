#pragma once

#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CCollider;
class CGameObject;
NS_END

NS_BEGIN(Client)

class CCollision_Manager final : public CBase
{
private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	/* 플레이어의 기본공격(검,주먹), 몬스터의 스킬 등, 콜라이더만 필요한 녀석들 */
	void Add_Collider_ToCollision(const _wstring& strColliderTag, class CCollider* pCollider);
	void Add_Object_ToCollision(const _wstring& strObjectTag, class CGameObject* pObject);

	/* Level의 업데이트에서 수행 */
	void Check_Collision(const _wstring strColliderTag, const _wstring strObjectTag, COLLISION_ID eCollisionID);

public:
	void Clear();

private:
	map<_wstring, vector<CCollider*>>		m_Colliders = {};
	map<_wstring, vector<CGameObject*>>		m_CollisionObjects = {};

public:
	static CCollision_Manager* Create();
	virtual void Free() override;
};

NS_END
