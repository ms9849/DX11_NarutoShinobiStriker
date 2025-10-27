#include "Boss_LightingRushState.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Player.h"
#include "Boss.h"
#include "FireBall.h"

#pragma region TRANSFER_STATE

#include "Boss_IdleState.h"
#include "Boss_StepState.h"
#include "Boss_LandState.h"

#pragma endregion

/* 이 상태에선 Compute Height 꺼야한다. */
CBoss_LightingRushState::CBoss_LightingRushState(CNavigation* pNavigation, CBoss* pBoss)
    : m_pBoss { pBoss }
    , m_pNavigationCom { pNavigation}
{
    Safe_AddRef(m_pNavigationCom);
}

void CBoss_LightingRushState::Start(_bool IsBlend)
{
    m_pBoss->Set_Flying(true);
    m_pBoss->Set_AnimIndex("CustomMan_Ninjutsu_RasenSenkoCBS_Start", 1.5f, true);

    m_pBoss->Get_Collider(TEXT("Com_Collider_Rush"))->Set_Active(true);


    m_eAnimState = ANIM_STATE::START;

    m_pBoss->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
}

CBossState* CBoss_LightingRushState::Update(_float fTimeDelta)
{
    CBossState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pBoss->Play_Animation(fTimeDelta);

    if (true == IsAnimFinished && ANIM_STATE::START == m_eAnimState)
    {
        /* 여기서 상태 변경하고 날아가게 해야한다 */
        m_pBoss->Set_AnimIndex("CustomMan_Ninjutsu_RasenSenkoCBS_Loop", 1.5f, true);
        m_eAnimState = ANIM_STATE::LOOP;

        /* 타겟으로 향하는 벡터 */
        _vector vBossToTarget = m_pPlayerTransformCom->Get_State(STATE::POSITION) - m_pBoss->Get_Transform()->Get_State(STATE::POSITION);

        _vector vDir = m_pBoss->Get_Transform()->Get_State(STATE::LOOK);
        _vector vRight = m_pBoss->Get_Transform()->Get_State(STATE::RIGHT);
        _vector vUp = m_pBoss->Get_Transform()->Get_State(STATE::UP);

        _vector RushDirs[4] = {};

        RushDirs[0] = XMVector3Normalize(XMVector3Normalize(vDir + vRight));
        RushDirs[1] = XMVector3Normalize(XMVector3Normalize(vDir - vRight));
        RushDirs[2] = XMVector3Normalize(vDir + vUp * 0.5f);
        RushDirs[3] = XMVector3Normalize(vDir - vUp * 0.5f);

        for (_int i = 0; i < 8; ++i)
        {
            _int dirIndex = i % 4; // 0,1,2,3 반복 (좌,우,상,하)

            _vector vOffset = RushDirs[dirIndex] * (i + 1);

            // 플레이어 쪽으로 나아가면서, 상하좌우로 흔들리며 돌진
            XMStoreFloat4(&m_RushPoints[i],
                XMVectorSetW(
                    m_pBoss->Get_Transform()->Get_State(STATE::POSITION)
                    + vBossToTarget / 9.f * (i + 1)
                    + vOffset,
                    1.f)
            );

            m_pNavigationCom->Find_CurrentCell(m_pBoss->Get_Transform()->Get_State(STATE::POSITION));
        }
    }

    if (ANIM_STATE::LOOP == m_eAnimState)
    {
        m_fTimeAcc += fTimeDelta;

        if (0.15f <= m_fTimeAcc && m_iRushCount >= 7)
        {
            m_pBoss->Get_Transform()->Set_State(STATE::POSITION, XMLoadFloat4(&m_RushPoints[m_iRushCount]) + XMVectorSet(0.f, 0.3f, 0.f, 0.f) * (m_iRushCount + 1));
            m_pNavigationCom->Compute_Height(m_pBoss->Get_Transform());
            m_pNavigationCom->Find_CurrentCell(m_pBoss->Get_Transform()->Get_State(STATE::POSITION));
            pNextState = CBoss_LandState::Create(m_pNavigationCom, m_pBoss);
        }
       
        else if (0.15f <= m_fTimeAcc)
        {   
            m_pBoss->Get_Transform()->Set_State(STATE::POSITION, XMLoadFloat4(&m_RushPoints[m_iRushCount]) + XMVectorSet(0.f, 0.3f, 0.f, 0.f) * (m_iRushCount + 1));
            m_iRushCount++;
            m_fTimeAcc = 0.f;
        }
    }

    return pNextState;
}

_bool CBoss_LightingRushState::End()
{
    m_pBoss->Set_Flying(false);
    m_pBoss->Get_Collider(TEXT("Com_Collider_Rush"))->Set_Active(false);

    return true;
}

CBoss_LightingRushState* CBoss_LightingRushState::Create(CNavigation* pNavigation, CBoss* pBoss)
{
    return new CBoss_LightingRushState(pNavigation, pBoss);
}

void CBoss_LightingRushState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
