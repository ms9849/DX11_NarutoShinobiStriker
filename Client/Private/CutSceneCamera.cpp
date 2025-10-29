#include "CutSceneCamera.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Player.h"

CCutSceneCamera::CCutSceneCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CCamera{ pDevice, pContext, ENUM_CLASS(eObjectID) }
    , m_pGameManager{ CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

CCutSceneCamera::CCutSceneCamera(const CCutSceneCamera& rhs)
    : CCamera{ rhs }
    , m_pGameManager{ CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

HRESULT CCutSceneCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCutSceneCamera::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CCutSceneCamera::Priority_Update(_float fTimeDelta)
{
    TRIGGER_TYPE eType = m_pGameManager->Get_CurrentTrigger();

    switch (eType)
    {
    case TRIGGER_TYPE::TUTORIAL_CLEAR :
        /* ³ª¹µÀÙ ¸¶À» ÀÔÀå ÄÆ¾À */
        KonohaVillage_CustScene(fTimeDelta);
        break;

    case TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_BOSS:
        /* º¸½º ÄÆ½Å ±¸ÇöÇØ¾ßÇÔ .*/
        Boss_CutScene(fTimeDelta);
        break;

    default:
        /* Æ©Åä¸®¾ó ÀÔÀå ÄÆ¾À */
        Tutorial_CutScene(fTimeDelta);
        break;

    }
    __super::Bind_Matrices();
}

void CCutSceneCamera::Update(_float fTimeDelta)
{
}

void CCutSceneCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCutSceneCamera::Render()
{
    return S_OK;
}

void CCutSceneCamera::OnChange(const _float4x4* pWorldMatrix)
{
    m_fTimeAcc = 0.f;

    TRIGGER_TYPE eType = m_pGameManager->Get_CurrentTrigger();

    switch (eType)
    {
    case TRIGGER_TYPE::TUTORIAL_CLEAR:
        /* ³ª¹µÀÙ ¸¶À» ÀÔÀå ÄÆ¾À ¼¼ÆÃ */
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(83.806f, 87.830f, -51.401f, 1.f));
        m_pGameManager->Set_AttackType_Visible(false);
        break;

    case TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_BOSS:
        /* º¸½º ÄÆ½Å ±¸ÇöÇØ¾ßÇÔ.*/
        m_pTargetTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Layer_Boss"),
            Engine::g_strTransformTag, 0));
        Safe_AddRef(m_pTargetTransform);
        break;

    default:
        /* Æ©Åä¸®¾ó ÀÔÀå ÄÆ¾À ¼¼ÆÃ */
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(71.447f, 76.337f, -60.446f, 1.f));
        m_pGameManager->Set_AttackType_Visible(false);
        break;
    }
}

void CCutSceneCamera::Tutorial_CutScene(_float fTimeDelta)
{
    _vector vStartPos = XMVectorSet(71.447f, 76.337f, -60.446f, 1.f);
    _vector vEndPos = XMVectorSet(84.477f, 66.518f, 24.298f, 1.f);
    _vector vLookPos = XMVectorSet(0.f, 0.f, 20.f, 1.f);

    _float CurDist = XMVectorGetX(XMVector3Length((m_pTransformCom->Get_State(STATE::POSITION) - vEndPos)));
    _float MaxDist = XMVectorGetX(XMVector3Length((vStartPos - vEndPos)));
    _float fRatio = (MaxDist - CurDist) / MaxDist;

    /* 0 ~ 1.. */
    _vector vLerpPos = XMVectorSetW(XMQuaternionSlerp(m_pTransformCom->Get_State(STATE::POSITION), vEndPos, fTimeDelta * 0.5f), 1.f);

    m_pTransformCom->Set_State(STATE::POSITION, vLerpPos);

    m_pGameManager->Get_PlayerPtr()->Set_Visible(false);

    if (fRatio >= 0.9f)
    {
        m_fTimeAcc += fTimeDelta;

        if (m_fTimeAcc >= 1.f)
        {
            m_pGameManager->Get_PlayerPtr()->Set_Visible(true);
            m_pGameManager->Set_CutScene_Visible(false);
            m_pGameManager->Set_AttackType_Visible(true);

            m_pGameInstance->PlayBGM(TEXT("TutorialMap_BackGround.wav"), 0.5f);
            m_pGameManager->Change_Camera(LEVEL::TUTORIAL, TEXT("Main_Camera"), nullptr);
        }
    }
    m_pTransformCom->LookAt(vLookPos);
}

void CCutSceneCamera::KonohaVillage_CustScene(_float fTimeDelta)
{
    _vector vStartPos = XMVectorSet(83.806f, 87.830f, -51.401f, 1.f);
    _vector vEndPos = XMVectorSet(-66.215f, 66.921f, -71.987f, 1.f);
    _vector vLookPos = XMVectorSet(-42.582f, 47.336f, 19.361f, 1.f);

    _float CurDist = XMVectorGetX(XMVector3Length((m_pTransformCom->Get_State(STATE::POSITION) - vEndPos)));
    _float MaxDist = XMVectorGetX(XMVector3Length((vStartPos - vEndPos)));
    _float fRatio = (MaxDist - CurDist) / MaxDist;

    /* 0 ~ 1.. */
    _vector vLerpPos = XMVectorSetW(XMQuaternionSlerp(m_pTransformCom->Get_State(STATE::POSITION), vEndPos, fTimeDelta * 0.5f), 1.f);

    m_pTransformCom->Set_State(STATE::POSITION, vLerpPos);

    m_pGameManager->Get_PlayerPtr()->Set_Visible(false);

    if (fRatio >= 0.9f)
    {
        m_fTimeAcc += fTimeDelta;

        if (m_fTimeAcc >= 1.f)
        {
            m_pGameManager->Get_PlayerPtr()->Set_Visible(true);
            m_pGameManager->Set_CutScene_Visible(false);
            m_pGameManager->Set_AttackType_Visible(true);
            m_pGameManager->Change_Camera(LEVEL::KONOHA_VILLAGE, TEXT("Main_Camera"), nullptr);
        }
    }
    m_pTransformCom->LookAt(vLookPos);

}

void CCutSceneCamera::Boss_CutScene(_float fTimeDelta)
{
    m_pGameManager->Get_PlayerPtr()->Set_Visible(false);

    /* ÀÏ´ÜÀº Á¤ÇØÁø À§Ä¡¿¡¼­ º¸°Ô¸¸ ÇÏÀÚ. */
    m_pTransformCom->Set_State(STATE::POSITION, 
       m_pTargetTransform->Get_State(STATE::POSITION) + m_pTargetTransform->Get_State(STATE::LOOK) * 4.0f + XMVectorSet(0.f, 1.9f, 0.f, 0.f));

    m_pTransformCom->LookAt(m_pTargetTransform->Get_State(STATE::POSITION) + XMVectorSet(0.f, 1.4f, 0.f, 0.f));

    m_fTimeAcc += fTimeDelta;

    if (m_fTimeAcc >= 3.f)
    {
        m_pGameManager->Get_PlayerPtr()->Set_Visible(true);
        m_pGameManager->Set_CutScene_Visible(false);
        m_pGameManager->Set_AttackType_Visible(true);
        m_pGameManager->OnTrigger(TRIGGER_TYPE::KONOHA_VILLAGE_BOSS_CUTSCENE_END);
        m_pGameManager->Change_Camera(LEVEL::KONOHA_VILLAGE, TEXT("Main_Camera"), nullptr);
    }

}

CCutSceneCamera* CCutSceneCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CCutSceneCamera* pInstance = new CCutSceneCamera(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CCutSceneCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCutSceneCamera::Clone(void* pArg)
{
    CCutSceneCamera* pInstance = new CCutSceneCamera(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CCutSceneCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCutSceneCamera::Free()
{
    __super::Free();

    Safe_Release(m_pGameManager);
    Safe_Release(m_pTargetTransform);
}
