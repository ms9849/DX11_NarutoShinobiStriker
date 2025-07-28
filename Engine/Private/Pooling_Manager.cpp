#include "Pooling_Manager.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Pooling.h"

CPooling_Manager::CPooling_Manager() 
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPooling_Manager::Initialize()
{
    return S_OK;
}

HRESULT CPooling_Manager::Add_GameObject_ToPool(CGameObject* pGameObject)
{
    /* 내부 컴포넌트의 Pooling Tag 찾아가서 문자열 가져와서 세팅해주기. */
    if (pGameObject->Find_Component(g_strPoolingTag) == nullptr)
        return E_FAIL;

    _wstring strPoolingTag = static_cast<CPooling*>(pGameObject->Find_Component(g_strPoolingTag))->Get_PoolingTag();
    auto iter = m_PoolObjects.find(strPoolingTag);

    if (iter == m_PoolObjects.end())
    {
        list<class CGameObject*> pPoolObjects = {};
        pPoolObjects.push_back(pGameObject);
        m_PoolObjects.emplace(strPoolingTag, pPoolObjects);
    }
    else
        iter->second.push_back(pGameObject);

    return S_OK;
}

HRESULT CPooling_Manager::Add_PoolingObject_ToLayer(const _wstring& strPoolingTag, _uint iLayerLevelIndex, const _wstring& strLayerTag)
{
    auto pPoolObjects = m_PoolObjects.find(strPoolingTag);

    if (pPoolObjects == m_PoolObjects.end())
        return E_FAIL;

    _int m_iCnt = 0;
    for (auto& iter : pPoolObjects->second)
    {
        /* 풀링 가능한 오브젝트가 아니라면 E_FAIL을 반환한다 */
        if (iter->Find_Component(g_strPoolingTag) == nullptr)
            return E_FAIL;

        /* 사용중이고, m_isDead가 True라면.. 이건 역할을 다하고 오브젝트 매니저에서 나온 녀석일 것. */
        if (static_cast<CPooling*>(iter->Find_Component(g_strPoolingTag))->IsUsing() == true && iter->isDead() == true)
        {
            static_cast<CPooling*>(iter->Find_Component(g_strPoolingTag))->Set_Using(false);
            iter->Set_Dead(false);
        }

        /* isDead가 false고, 오브젝트 매니저에서 사용중이 아니라면 오브젝트 매니저에 추가 */
        if (static_cast<CPooling*>(iter->Find_Component(g_strPoolingTag))->IsUsing() == false && iter->isDead() == false)
        {
            Safe_AddRef(iter);
            static_cast<CPooling*>(iter->Find_Component(g_strPoolingTag))->Set_Using(true);

            m_pGameInstance->Add_Clone_ToLayer(iter, iLayerLevelIndex, strLayerTag);
            break;
        }

        m_iCnt++;
    }

    return S_OK;
}

CPooling_Manager* CPooling_Manager::Create()
{
    CPooling_Manager* pInstance = new CPooling_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Create Failed : Pooling Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPooling_Manager::Free()
{
    __super::Free();

    for (auto& pPoolObjects : m_PoolObjects)
    {
        for (auto& pObject : pPoolObjects.second)
            Safe_Release(pObject);

        pPoolObjects.second.clear();
    }

    m_PoolObjects.clear();

    Safe_Release(m_pGameInstance);
}
