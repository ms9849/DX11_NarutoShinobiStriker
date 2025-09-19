#include "LockOn_Manager.h"

#include "GameInstance.h"

CLockOn_Manager::CLockOn_Manager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

void CLockOn_Manager::Reset_AttackTime()
{
    m_fAttackTimeAcc = 0.f;
}

HRESULT CLockOn_Manager::Initialize()
{
    return S_OK;
}

void CLockOn_Manager::Update(_float fTimeDelta)
{
    m_fAttackTimeAcc += fTimeDelta;
}

CTransform* CLockOn_Manager::Calc_Target()
{
    /* 만약 m_fAttackTime 이내에 공격한 적이 있었다면 리턴*/
    if (m_fAttackTimeAcc <= m_fAttackTime)
        return nullptr;

    /* 현재 레벨의 모든 몬스터 순회 */
    _int iNumTargets = m_pGameInstance->Get_LayerSize(m_pGameInstance->Get_LevelID(), TEXT("Layer_Monster"));

    _float      fDist, fMinDist = { FLT_MAX };
    CTransform* pNearestTransform = {};
    _float4     vCamPosition = *m_pGameInstance->Get_CamState(STATE::POSITION);

    for (_int i = 0; i < iNumTargets; ++i)
    {
        CTransform* pSourTransform = m_pGameInstance->Get_GameObject(m_pGameInstance->Get_LevelID(), TEXT("Layer_Monster"), i)->Get_Transform();
    
        fDist = XMVectorGetX(XMVector4Length(XMLoadFloat4(&vCamPosition) - pSourTransform->Get_State(STATE::POSITION)));

        /* 가장 가까운 거리의 트랜스폼을 찾아낸다. */
        if (fDist < fMinDist)
        {
            pNearestTransform = pSourTransform;
            fMinDist = fDist;
        }
    }

    return pNearestTransform;
}

CLockOn_Manager* CLockOn_Manager::Create()
{
    CLockOn_Manager* pInstance = new CLockOn_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Create Failed : LockOn Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLockOn_Manager::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
