#include "Trigger_Manager.h"

#include "GameManager.h"
#include "GameInstance.h"
#include "TriggerBox.h"

CTrigger_Manager::CTrigger_Manager()
    : m_pGameManager { CGameManager::GetInstance() }
    , m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
    Safe_AddRef(m_pGameInstance);
    ZeroMemory(m_IsTriggerActivated, sizeof(_bool) * ENUM_CLASS(TRIGGER_TYPE::END));
}

HRESULT CTrigger_Manager::OnTrigger(TRIGGER_TYPE eTriggerType)
{
    if (TRIGGER_TYPE::TUTORIAL_KAKASHI_TALK == eTriggerType && false == m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::TUTORIAL_KAKASHI_TALK)])
    {
        CTriggerBox::TRIGGER_BOX_DESC Desc;
        Desc.eTriggerType = TRIGGER_TYPE::TUTORIAL_SPAWNER_01;
        Desc.fRadius = 5.f;
        Desc.vCenter = _float3(0.f, 0.f, 0.f);
        Desc.vPosition = _float3(1.222f, -0.021f, -30.5f);

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MonsterSpawner"),
            ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Layer_Trigger"), &Desc)))
            return E_FAIL;

        m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::TUTORIAL_KAKASHI_TALK)] = true;
    }

    else if (TRIGGER_TYPE::TUTORIAL_SPAWNER_01 == eTriggerType && false == m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::TUTORIAL_SPAWNER_01)])
    {
        if(FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_WhiteJetsu"),
            ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Layer_Monster"))))
            return E_FAIL;

        m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::TUTORIAL_SPAWNER_01)] = true;
    }

    else if (TRIGGER_TYPE::TUTORIAL_CLEAR == eTriggerType &&  false == m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::TUTORIAL_CLEAR)])
    {
        m_pGameInstance->Request_LevelChange();

        m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::TUTORIAL_CLEAR)] = true;
    }

    m_eCurrentTrigger = eTriggerType;

    return S_OK;
}

CTrigger_Manager* CTrigger_Manager::Create()
{
    return new CTrigger_Manager();
}

void CTrigger_Manager::Free()
{
    __super::Free();

    Safe_Release(m_pGameManager);
    Safe_Release(m_pGameInstance);
}
