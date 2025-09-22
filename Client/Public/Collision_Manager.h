#pragma once

#include "Client_Defines.h"
#include "Base.h"

/*
구조 다시 고민해봐야 함.
1. 콜라이더의 정보는 어떻게 받아올건지
2. 콜라이더 객체 등록할떄 자기 자신도 등록하게 할 것
3. 콜라이더 데스크 세팅 진짜 이게 최선인지
*/

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
	void Update();
	/* 플레이어의 기본공격(검,주먹), 몬스터의 스킬 등, 콜라이더만 필요한 녀석들 */
	void Add_Collider_ToCollision(const _wstring& strColliderTag, COLLIDER_HANDLE_ID eHandleID, class CCollider* pCollider);
	void Add_Object_ToCollision(const _wstring& strObjectTag, CGameObject* pGameObject, CCollider* pCollider);

	/* Level의 업데이트에서 수행 */
	/* 어떤 충돌체에 충돌했냐에 따라 다른 로직을 보내야 한다. */
	/* COLLISION_DESC를 만들기 ..*/
	void Check_Collision(const _wstring strColliderTag, const _wstring strObjectTag, COLLISION_TYPE eColType);

public:
	void Clear();

private:
	map<_wstring, vector<pair<COLLIDER_HANDLE_ID, CCollider*>>>	m_Colliders = {};
	map<_wstring, vector<pair<CGameObject*, CCollider*>>>		m_CollisionObjects = {};

	class CGameManager* m_pGameManager = { nullptr };

public:
	static CCollision_Manager* Create();
	virtual void Free() override;
};

NS_END

//NS_BEGIN(Client)
//
//class CCollision_Manager final : public CBase
//{
//private:
//	CCollision_Manager();
//	virtual ~CCollision_Manager() = default;
//
//public:
//	/* 플레이어의 기본공격(검,주먹), 몬스터의 스킬 등, 콜라이더만 필요한 녀석들 */
//	void Add_Collider_ToCollision(const _wstring& strColliderTag, class CCollider* pCollider);
//	void Add_Object_ToCollision(const _wstring& strObjectTag, class CGameObject* pObject);
//
//	/* Level의 업데이트에서 수행 */
//	/* 어떤 충돌체에 충돌했냐에 따라 다른 로직을 보내야 한다. */
//	/* COLLISION_DESC를 만들기 ..*/
//	void Check_Collision(const _wstring strColliderTag, const _wstring strObjectTag);
//
//public:
//	void Clear();
//
//private:
//	map<_wstring, vector<CCollider*>>		m_Colliders = {};
//	map<_wstring, vector<CGameObject*>>		m_CollisionObjects = {};
//
//public:
//	static CCollision_Manager* Create();
//	virtual void Free() override;
//};
//
//NS_END
