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

    TRIGGER_TYPE eType = m_pGameManager->Get_CurrentTrigger();

    switch (eType)
    {
    case TRIGGER_TYPE::TUTORIAL_CLEAR:
        /* ³ª¹µÀÙ ¸¶À» ÀÔÀå ÄÆ¾À ¼¼ÆÃ */
        break;

    case TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_BOSS:
        /* º¸½º ÄÆ½Å ±¸ÇöÇØ¾ßÇÔ.*/
        break;

    default:
        /* Æ©Åä¸®¾ó ÀÔÀå ÄÆ¾À ¼¼ÆÃ */
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(71.447f, 76.337f, -60.446f, 1.f));
        m_pGameManager->Set_AttackType_Visible(false);
        break;
    }


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

void CCutSceneCamera::Tutorial_CutScene(_float fTimeDelta)
{
    _float CurDist = XMVectorGetX(XMVector3Length((m_pTransformCom->Get_State(STATE::POSITION) - XMVectorSet(84.477f, 66.518f, 24.298f, 1.f))));
    _float MaxDist = XMVectorGetX(XMVector3Length((XMVectorSet(71.447f, 76.337f, -60.446f, 1.f) - XMVectorSet(84.477f, 66.518f, 24.298f, 1.f))));
    _float fRatio = (MaxDist - CurDist) / MaxDist;

    /* 0 ~ 1.. */
    _vector vLerpPos = XMVectorSetW(XMQuaternionSlerp(m_pTransformCom->Get_State(STATE::POSITION), XMVectorSet(84.477f, 66.518f, 24.298f, 1.f), fTimeDelta * 0.5f), 1.f);

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
            m_pGameManager->Change_Camera(LEVEL::TUTORIAL, TEXT("Main_Camera"), nullptr);
        }
    }
    m_pTransformCom->LookAt(XMVectorSet(0.f, 0.f, 20.f, 1.f));
}

void CCutSceneCamera::KonohaVillage_CustScene(_float fTimeDelta)
{
}

void CCutSceneCamera::Boss_CutScene(_float fTimeDelta)
{
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
}
