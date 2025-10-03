#include "Boss_LightingRushState.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Player.h"
#include "Boss.h"
#include "FireBall.h"

#pragma region TRANSFER_STATE

#include "Boss_IdleState.h"
#include "Boss_StepState.h"

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
    m_eAnimState = ANIM_STATE::START;
    /* 타겟 위치 */
    XMStoreFloat3(&m_vTargetPos, m_pPlayerTransformCom->Get_State(STATE::POSITION));


    /* 타겟에게 돌진할때 사용할 벡터 */
    _vector vDir, vRight;
    
    vDir = m_pPlayerTransformCom->Get_State(STATE::POSITION) - m_pBoss->Get_Transform()->Get_State(STATE::POSITION);
    vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_pPlayerTransformCom->Get_State(STATE::POSITION) - m_pBoss->Get_Transform()->Get_State(STATE::POSITION));

    XMStoreFloat3(&m_RushDirs[0], XMVector3Normalize(XMVector3Normalize(vDir + vRight) + XMVectorSet(0.f, 0.2f, 0.f, 0.f)));
    XMStoreFloat3(&m_RushDirs[1], XMVector3Normalize(XMVector3Normalize(vDir - vRight) + XMVectorSet(0.f, 0.2f, 0.f, 0.f)));
}

CBossState* CBoss_LightingRushState::Update(_float fTimeDelta)
{
    CBossState* pNextState = { nullptr };
    _bool IsAnimFinished = m_pBoss->Play_Animation(fTimeDelta);

    /* 돌진 횟수를 제한, 미리 거리에 따른 Offset을 계산해놓는다면? */
    if (true == IsAnimFinished && ANIM_STATE::START == m_eAnimState)
    {
        /* 여기서 상태 변경하고 날아가게 해야한다 */
        m_pBoss->Set_AnimIndex("CustomMan_Ninjutsu_RasenSenkoCBS_Loop", 1.5f, false);
    }

    if (ANIM_STATE::LOOP == m_eAnimState)
    {
        _float fTargetDist =  XMVectorGetX(m_pBoss->Get_Transform()->Get_State(STATE::POSITION) - XMLoadFloat3(&m_vTargetPos));
        if (fTargetDist <= 5.f)
        {
            /* 임시로 플레이어 pos로 설정. 무조건 맞긴한다.. */
            m_pBoss->Get_Transform()->Set_State(STATE::POSITION, m_pPlayerTransformCom->Get_State(STATE::POSITION));
        }

        m_fTimeAcc += fTimeDelta;
       
        if (0.2f <= m_fTimeAcc)
        {
            m_pBoss->Get_Transform()->Set_State(STATE::POSITION, m_pBoss->Get_Transform()->Get_State(STATE::POSITION) + XMLoadFloat3(&m_RushDirs[m_iRushDir]) * m_iRushCount);

            m_fTimeAcc = 0.f;
            /* 1이였다면 0, 0이였다면 1로 */
            m_iRushDir = 1 - m_iRushDir;
            m_iRushCount++;
        }
    }

    return pNextState;
}

_bool CBoss_LightingRushState::End()
{
    m_pBoss->Set_Flying(false);

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
