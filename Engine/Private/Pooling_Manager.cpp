#include "Pooling_Manager.h"

#include "GameInstance.h"

#include "GameObject.h"
#include "Pooling.h"
#include "IMGUI_Manager.h"

CPooling_Manager::CPooling_Manager() 
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPooling_Manager::Initialize(_uint iNumLevels)
{
    m_iNumLevels = iNumLevels;

    m_PoolObjects = new map<_wstring, list<class CGameObject*>>[m_iNumLevels];
    m_PoolCounts = new map<_wstring, _uint>[m_iNumLevels];

    return S_OK;
}

void CPooling_Manager::Update()
{
    _wstring strPoolingTag;
    CPooling* pPoolingCom;

    for (size_t i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& pPoolObjects : m_PoolObjects[i])
        {
            strPoolingTag = pPoolObjects.first;

            for (auto& iter : pPoolObjects.second)
            {
                pPoolingCom = static_cast<CPooling*>(iter->Find_Component(g_strPoolingTag));
                /* 사용중이고, m_isDead가 True라면, 역할을 다하고 오브젝트 매니저에서 나온 녀석일 것. */
                if (pPoolingCom->IsUsing() == true && iter->isDead() == true)
                {
                    pPoolingCom->Set_Using(false);
                    iter->Set_Dead(false);
                    m_PoolCounts[i].find(strPoolingTag)->second--;
                }
            }
        }
    }
}

void CPooling_Manager::Clear(_uint iLevelIndex)
{
    for (auto& pPoolObjects : m_PoolObjects[iLevelIndex])
    {
        for (auto& pObject : pPoolObjects.second)
            Safe_Release(pObject);

        pPoolObjects.second.clear();
    }

    m_PoolObjects[iLevelIndex].clear();
}

HRESULT CPooling_Manager::Add_GameObject_ToPool(_uint iLevelIndex, CGameObject* pGameObject)
{
    /* 내부 컴포넌트의 Pooling Tag 찾아가서 문자열 가져와서 세팅해주기. */
    if (pGameObject->Find_Component(g_strPoolingTag) == nullptr)
        return E_FAIL;

    _wstring strPoolingTag = static_cast<CPooling*>(pGameObject->Find_Component(g_strPoolingTag))->Get_PoolingTag();
    auto iter = m_PoolObjects[iLevelIndex].find(strPoolingTag);

    if (iter == m_PoolObjects[iLevelIndex].end())
    {
        list<class CGameObject*> pPoolObjects = {};
        pPoolObjects.push_back(pGameObject);
        m_PoolObjects[iLevelIndex].emplace(strPoolingTag, pPoolObjects);
        m_PoolCounts[iLevelIndex].emplace(strPoolingTag, 0);
    }
    else
        iter->second.push_back(pGameObject);
        
    return S_OK;
}

HRESULT CPooling_Manager::Add_PoolingObject_ToLayer(const _wstring& strPoolingTag, _uint iPoolingOjbectLevelIndex, _uint iLayerLevelIndex, const _wstring& strLayerTag)
{
    CPooling* pPoolingCom;

    auto pPoolObjects = m_PoolObjects[iPoolingOjbectLevelIndex].find(strPoolingTag);

    if (pPoolObjects == m_PoolObjects[iPoolingOjbectLevelIndex].end())
        return E_FAIL;

    for (auto& pPoolObject : pPoolObjects->second)
    {
        pPoolingCom = static_cast<CPooling*>(pPoolObject->Find_Component(g_strPoolingTag));
        /* 풀링 가능한 오브젝트가 아니라면 E_FAIL을 반환한다 */
        if (pPoolingCom == nullptr)
            return E_FAIL;

        /* isDead가 false고, 오브젝트 매니저에서 사용중이 아니라면 오브젝트 매니저에 추가 */
        if (pPoolingCom->IsUsing() == false && pPoolObject->isDead() == false)
        {
            Safe_AddRef(pPoolObject);
            pPoolingCom->Set_Using(true);

            m_pGameInstance->Add_Clone_ToLayer(pPoolObject, iLayerLevelIndex, strLayerTag);
            m_PoolCounts[iPoolingOjbectLevelIndex].find(strPoolingTag)->second++;
            break;
        }
    }

    return S_OK;
}

CPooling_Manager* CPooling_Manager::Create(_uint iNumLevels)
{
    CPooling_Manager* pInstance = new CPooling_Manager();

    if (FAILED(pInstance->Initialize(iNumLevels)))
    {
        MSG_BOX("Create Failed : Pooling Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPooling_Manager::Free()
{
    __super::Free();

    for (size_t i = 0; i< m_iNumLevels; ++i)
    {
        for (auto& pPoolObjects : m_PoolObjects[i])
        {
            for (auto& pObject : pPoolObjects.second)
                Safe_Release(pObject);

            pPoolObjects.second.clear();
        }

        m_PoolObjects[i].clear();
    }

    Safe_Delete_Array(m_PoolObjects);
    Safe_Delete_Array(m_PoolCounts);

    Safe_Release(m_pGameInstance);
}
