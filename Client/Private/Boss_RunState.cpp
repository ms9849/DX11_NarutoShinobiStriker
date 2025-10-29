#include "Boss_RunState.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Player.h"
#include "Boss.h"

#pragma region TRANSFER_STATE

#include "Boss_WalkState.h"
#include "Boss_SpinKickState.h"
#include "Boss_FireballState.h"
#include "Boss_WoodHandState.h"
//#include "Boss_SlidingKunaiState.h"

#pragma endregion

CBoss_RunState::CBoss_RunState(CNavigation* pNavigation, CBoss* pBoss)
    : m_pNavigationCom{ pNavigation }
    , m_pBoss { pBoss }
{
    Safe_AddRef(m_pNavigationCom);
}

void CBoss_RunState::Start(_bool IsBlend)
{
    m_pBoss->Set_AnimIndex("CustomMan_Run_Loop", 0.8f, IsBlend, 0.1f);
}

CBossState* CBoss_RunState::Update(_float fTimeDelta)
{
    CBossState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pBoss->Play_Animation(fTimeDelta);
    _float fDist = XMVectorGetX(XMVector3Length(m_pBoss->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));

    /* 스킬 사용 로직 */

    /* 돌진 패턴은 거리 20 이상일때만 발동한다. */

    if (true == m_pBoss->Use_Skill(CBoss::BOSS_SKILL::SPIN_KICK) && fDist <= 20.f)
    {
        pNextState = CBoss_SpinKickState::Create(m_pNavigationCom, m_pBoss);
    }
    else if (true == m_pBoss->Use_Skill(CBoss::BOSS_SKILL::WOODHAND) && fDist >= 8.f)
    {
        pNextState = CBoss_WoodHandState::Create(m_pNavigationCom, m_pBoss);
    }
    else if (true == m_pBoss->Use_Skill(CBoss::BOSS_SKILL::FIREBALL) && fDist >= 5.f)
    {
        pNextState = CBoss_FireBallState::Create(m_pNavigationCom, m_pBoss);
    }
    //else if (true == m_pBoss->Use_Sliding() && fDist > 5.f)
    //{
    //    _float fRandom = m_pGameInstance->Random_Normal();

    //    if (fRandom > 0.5f)
    //        pNextState = CPajama_SlidingKunaiState::Create(m_pNavigationCom, m_pBoss, CPajama_SlidingKunaiState::ANIM_STATE::LEFT);
    //    else
    //        pNextState = CPajama_SlidingKunaiState::Create(m_pNavigationCom, m_pBoss, CPajama_SlidingKunaiState::ANIM_STATE::RIGHT);
    //}
    /* 달리기 로직 */
    else if (fDist >= 10.f)
    {
        m_pBoss->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
        m_pBoss->Get_Transform()->Chase_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION), fTimeDelta, m_pNavigationCom, 1.f);
    }
    /* 걷는 로직 */
    else if (fDist < 10.f)
    {
        pNextState = CBoss_WalkState::Create(m_pNavigationCom, m_pBoss);
    }

    return pNextState;
}

_bool CBoss_RunState::End()
{
    return true;
}

CBoss_RunState* CBoss_RunState::Create(CNavigation* pNavigation, CBoss* pBoss)
{
    return new CBoss_RunState(pNavigation, pBoss);
}

void CBoss_RunState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
