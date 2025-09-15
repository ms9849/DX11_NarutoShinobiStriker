#include "Collision_Manager.h"

#include "Collider.h"
#include "GameObject.h"

CCollision_Manager::CCollision_Manager()
{
}

void CCollision_Manager::Add_Collider_ToCollision(const _wstring& strLayerTag, CCollider* pCollider)
{
    auto iter = m_Colliders.find(strLayerTag);

    /* 해당 태그 비어있으면 벡터 추가. */
	if (iter == m_Colliders.end())
	{
        vector<CCollider*> Colliders;
        Colliders.push_back(pCollider);
        m_Colliders.emplace(strLayerTag, Colliders);
	}
    /* 아니면 바로 삽입해서 추가 */
    else
        iter->second.push_back(pCollider);
}

void CCollision_Manager::Add_Object_ToCollision(const _wstring& strLayerTag, CGameObject* pGameObject)
{
    auto iter = m_CollisionObjects.find(strLayerTag);

    /* 해당 태그 비어있으면 벡터 추가. */
    if (iter == m_CollisionObjects.end())
    {
        vector<CGameObject*> GameObjects;
        GameObjects.push_back(pGameObject);
        m_CollisionObjects.emplace(strLayerTag, GameObjects);
    }
    /* 아니면 바로 삽입해서 추가 */
    else
        iter->second.push_back(pGameObject);
}

void CCollision_Manager::Check_Collision(const _wstring strColliderTag, const _wstring strObjectTag, COLLISION_ID eCollisionID)
{
    /* 콜리전 아이디 따라 캐스팅 다르게 해줄것. */
    /* Player, Enemy 단으로 나누면 된다. */
    /* 로직은 내일 충돌 마무리 된 뒤에 설정할 것 */

}

void CCollision_Manager::Clear()
{
    for (auto& Pair : m_Colliders)
    {
        for (auto& pCollider : Pair.second)
        {
			Safe_Release(pCollider);
        }
        Pair.second.clear();
    }

    m_Colliders.clear();

    for (auto& Pair : m_CollisionObjects)
    {
        for (auto& pGameObject : Pair.second)
        {
            Safe_Release(pGameObject);
        }
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
            Safe_Release(pCollider);
        }
        Pair.second.clear();
    }

    m_Colliders.clear();

    for (auto& Pair : m_CollisionObjects)
    {
        for (auto& pGameObject : Pair.second)
        {
            Safe_Release(pGameObject);
        }
    }

    m_CollisionObjects.clear();
}
