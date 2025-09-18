#include "Collision_Manager.h"

#include "Collider.h"
#include "GameObject.h"

#include "Enemy.h"
#include "Player.h"

CCollision_Manager::CCollision_Manager()
{
}

void CCollision_Manager::Update()
{
    /* 매 프레임 연산이 끝나면 추가했던 모든 객체및 콜라이더들은 제거한다. */
    Clear();
}

void CCollision_Manager::Add_Collider_ToCollision(const _wstring& strColliderTag, COLLIDER_HANDLE_ID eHandleID, CCollider* pCollider)
{
    auto iter = m_Colliders.find(strColliderTag);

    /* 해당 태그 비어있으면 벡터 추가. */
    if (iter == m_Colliders.end())
    {
        vector<pair<COLLIDER_HANDLE_ID, CCollider*>> Colliders;
        Colliders.push_back(make_pair(eHandleID, pCollider));
        m_Colliders.emplace(strColliderTag, Colliders);
        //m_Colliders.emplace(strColliderTag, make_pair(eHandleID, pCollider));
    }
    else
        iter->second.push_back(make_pair(eHandleID, pCollider));

    /* 아니면 바로 삽입해서 추가 */

    Safe_AddRef(pCollider);
}

void CCollision_Manager::Add_Object_ToCollision(const _wstring& strObjectTag, CGameObject* pGameObject, CCollider* pCollider)
{
    auto iter = m_CollisionObjects.find(strObjectTag);

    /* 해당 태그 비어있으면 벡터 추가. */
    if (iter == m_CollisionObjects.end())
    {
        vector<pair<CGameObject*, CCollider*>> GameObjects;
        GameObjects.push_back(make_pair(pGameObject, pCollider));
        m_CollisionObjects.emplace(strObjectTag, GameObjects);
        //m_CollisionObjects.emplace(strObjectTag, make_pair(pGameObject, pCollider));
    }
    else
        iter->second.push_back(make_pair(pGameObject, pCollider));
    /* 아니면 바로 삽입해서 추가 */

    Safe_AddRef(pGameObject);
    Safe_AddRef(pCollider);
}

void CCollision_Manager::Check_Collision(const _wstring strColliderTag, const _wstring strObjectTag, COLLISION_TYPE eColType)
{
    /* 콜리전 아이디 따라 캐스팅 다르게 해줄것. */
    /* Player, Enemy 두개 로 나누면 된다. */
    auto Coliter = m_Colliders.find(strColliderTag);
    if (m_Colliders.end() == Coliter)
        return;

    vector<pair<COLLIDER_HANDLE_ID, CCollider*>> Colliders = Coliter->second;

    auto Objiter = m_CollisionObjects.find(strObjectTag);
    if (m_CollisionObjects.end() == Objiter)
        return;

    vector<pair<CGameObject*, CCollider*>> GameObjects = Objiter->second;

    for (auto& pCollider : Colliders)
    {
        for (auto& pCollisionObject : GameObjects)
        {
            /* 만약 콜라이더가 충돌했다면 */
            if (true == pCollisionObject.second->Get_Active()
                && true == pCollider.second->Get_Active()
                && true == pCollisionObject.second->InterSect(pCollider.second))
            {
                /* 충돌한 콜라이더도 처리해야함 */
                /* 추후 나선 수리검 & 카무이 & 대교탄같은건 냅둬야함 */
                if(pCollider.first != COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_KAMUI &&
                    pCollider.first != COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENSHURIKEN_EXPLODE && 
                    pCollider.first != COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_BIGSHARK)

                    pCollider.second->Set_Active(false);


                /* 미리 Client_Defines에 선언해둔 태그에 따라 알맞는 콜리전 선언. */
                if(COLLISION_TYPE::MONSTER == eColType)
                    static_cast<CEnemy*>(pCollisionObject.first)->OnCollision(pCollider.first);
                else if(COLLISION_TYPE::PLAYER == eColType)
                    static_cast<CPlayer*>(pCollisionObject.first)->OnCollision(pCollider.first);
            }
        }
    }
}

void CCollision_Manager::Clear()
{
    /* 콜라이더만 사용하니까. */
    for (auto& Pair : m_Colliders)
    {
        for (auto& pCollider : Pair.second)
        {
			Safe_Release(pCollider.second);
        }
        Pair.second.clear();
    }

    m_Colliders.clear();

    /* 게임 오브젝트, 콜라이더 쌍 */
    for (auto& Pair : m_CollisionObjects)
    {
        for (auto& pGameObject : Pair.second)
        {
            Safe_Release(pGameObject.first);
            Safe_Release(pGameObject.second);
        }
        Pair.second.clear();
    }

    m_CollisionObjects.clear();
}

CCollision_Manager* CCollision_Manager::Create()
{
    return new CCollision_Manager();
}

void CCollision_Manager::Free()
{
    __super::Free();

    for (auto& Pair : m_Colliders)
    {
        for (auto& pCollider : Pair.second)
        {
            Safe_Release(pCollider.second);
        }
        Pair.second.clear();
    }

    m_Colliders.clear();

    for (auto& Pair : m_CollisionObjects)
    {
        for (auto& pGameObject : Pair.second)
        {
            Safe_Release(pGameObject.first);
            Safe_Release(pGameObject.second);
        }
        Pair.second.clear();
    }

    m_CollisionObjects.clear();
}
