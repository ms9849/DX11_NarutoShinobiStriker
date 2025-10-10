#include "Trigger_Manager.h"

#include "GameManager.h"
#include "GameInstance.h"
#include "TriggerBox.h"
#include "Boss.h"

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
    m_eCurrentTrigger = eTriggerType;

    if (TRIGGER_TYPE::TUTORIAL_KAKASHI_TALK == eTriggerType && false == m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::TUTORIAL_KAKASHI_TALK)])
    {
        CEventObject::EVENT_COLLIDER_DESC Desc;
        Desc.eTriggerType = TRIGGER_TYPE::TUTORIAL_SPAWNER_01;
        Desc.fRadius = 5.f;
        Desc.vCenter = _float3(0.f, 0.f, 0.f);
        Desc.vPosition = _float3(1.222f, 0.679f, -30.5f);

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MonsterSpawner"),
            ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Layer_Trigger"), &Desc)))
            return E_FAIL;

        m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::TUTORIAL_KAKASHI_TALK)] = true;
    }

    else if (TRIGGER_TYPE::TUTORIAL_SPAWNER_01 == eTriggerType && false == m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::TUTORIAL_SPAWNER_01)])
    {
        /* 보스 테스트용 코드. 주석 풀었다 걸었다 하면서 체크 계속할 것. */
        //if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Boss"),
        //	ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Layer_Monster"))))
        //	return E_FAIL;

        if(FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_WhiteJetsu"),
            ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Layer_Monster"))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Bird"),
            ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Layer_Monster"))))
            return E_FAIL;

        m_pGameManager->AlertPanel_Start_FadeIn(TEXT("적을 쓰러뜨려라!"));
        m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::TUTORIAL_SPAWNER_01)] = true;
    }

    else if (TRIGGER_TYPE::TUTORIAL_CLEAR == eTriggerType &&  false == m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::TUTORIAL_CLEAR)])
    {
        m_pGameInstance->Request_LevelChange();

        m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::TUTORIAL_CLEAR)] = true;
    }

    else if (TRIGGER_TYPE::KONOHA_VILLAGE_KAKASHI_TALK_1 == eTriggerType && false == m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::KONOHA_VILLAGE_KAKASHI_TALK_1)])
    {
        CEventObject::EVENT_COLLIDER_DESC Desc;
        Desc.eTriggerType = TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_01;
        Desc.fRadius = 5.f;
        Desc.vCenter = _float3(0.f, 0.f, 0.f);
        Desc.vPosition = _float3(93.838f, 7.775f, -61.551f);

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MonsterSpawner"),
            ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Layer_Trigger"), &Desc)))
            return E_FAIL;

        m_pGameManager->AlertPanel_Start_FadeIn(TEXT("정해진 구역으로 이동하라!"));
        m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::KONOHA_VILLAGE_KAKASHI_TALK_1)] = true;
    }

    else if (TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_01 == eTriggerType && false == m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_01)])
    {
        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_WhiteJetsu"),
            ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Layer_Monster"))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Bird"),
            ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Layer_Monster"))))
            return E_FAIL;

        m_pGameManager->AlertPanel_Start_FadeIn(TEXT("적을 쓰러뜨려라!"));
        m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_01)] = true;
    }
    /* 중간 보스 스포너 생성*/
    else if (TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_01_CLEAR == eTriggerType && false == m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_01_CLEAR)])
    {
        CEventObject::EVENT_COLLIDER_DESC Desc;
        Desc.eTriggerType = TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_02;
        Desc.fRadius = 5.f;
        Desc.vCenter = _float3(0.f, 0.f, 0.f);
        Desc.vPosition = _float3(75.629f, 12.7f, 24.488f);

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MonsterSpawner"),
            ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Layer_Trigger"), &Desc)))
            return E_FAIL;

        m_pGameManager->AlertPanel_Start_FadeIn(TEXT("정해진 구역으로 이동하라!"));
        m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_01_CLEAR)] = true;

    }
    /* 중간보스 생성 */
    else if (TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_02 == eTriggerType && false == m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_02)])
    {
        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Boxer"),
            ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Layer_Monster"))))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Pajama"),
            ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Layer_Monster"))))
            return E_FAIL;

        m_pGameManager->AlertPanel_Start_FadeIn(TEXT("적을 쓰러뜨려라!"));
        m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_02)] = true;
    }
    /* 중간보스 클리어 */
    else if (TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_02_CLEAR == eTriggerType && false == m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_02_CLEAR)])
    {
        CEventObject::EVENT_COLLIDER_DESC Desc;
        Desc.eTriggerType = TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_BOSS;
        Desc.fRadius = 5.f;
        Desc.vCenter = _float3(0.f, 0.f, 0.f);
        Desc.vPosition = _float3(-82.641f, 28.700f, 59.220f);

        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MonsterSpawner"),
            ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Layer_Trigger"), &Desc)))
            return E_FAIL;

        m_pGameManager->AlertPanel_Start_FadeIn(TEXT("정해진 구역으로 이동하라!"));
        m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_02_CLEAR)] = true;
    }
    /* 보스 스포너. 건드리면 컷씬 출력 */
    else if (TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_BOSS == eTriggerType && false == m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_BOSS)])
    {
        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_GameObject_Boss"),
        ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Layer_Boss"))))
            return E_FAIL;

        m_pGameManager->Set_CutScene_Visible(true);
        m_pGameManager->Change_Camera(LEVEL::KONOHA_VILLAGE, TEXT("CutScene_Camera"), nullptr);

        m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_BOSS)] = true;
    }
    /* 보스와의 전투 시작 이벤트 */
    else if (TRIGGER_TYPE::KONOHA_VILLAGE_BOSS_CUTSCENE_END == eTriggerType && false == m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::KONOHA_VILLAGE_BOSS_CUTSCENE_END)])
    {
        static_cast<CBoss*>(m_pGameInstance->Get_GameObject(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Layer_Boss"), 0))->Start_Battle();
        m_pGameManager->AlertPanel_Start_FadeIn(TEXT("시연회를 위해 적을 쓰러뜨려라!"));
        m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::KONOHA_VILLAGE_BOSS_CUTSCENE_END)] = true;
    }
    /* 끝..*/
    else if (TRIGGER_TYPE::CLEAR == eTriggerType && false == m_IsTriggerActivated[ENUM_CLASS(TRIGGER_TYPE::CLEAR)])
    {
    }

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
