#include "Boss_FireBallState.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Player.h"
#include "Boss.h"
#include "FireBall.h"

#pragma region TRANSFER_STATE

#include "Boss_IdleState.h"
//#include "Boss_SlidingKunaiState.h"
#include "Boss_StepState.h"

#pragma endregion

CBoss_FireBallState::CBoss_FireBallState(CNavigation* pNavigation, CBoss* pBoss)
	: m_pNavigationCom{ pNavigation }
	, m_pBoss { pBoss }
{
    Safe_AddRef(m_pNavigationCom);
}

void CBoss_FireBallState::Start(_bool IsBlend)
{
	m_pBoss->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
	m_pBoss->Set_AnimIndex("CustomMan_Ninjutsu_Fireball_Lv3", 1.4f, IsBlend, 0.1f);
}

CBossState* CBoss_FireBallState::Update(_float fTimeDelta)
{
    CBossState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pBoss->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pBoss->Get_AnimProgress();

    if (false == m_IsFireballThrow && fAnimProgress >= 0.7f)
    {
        _vector vLook = m_pPlayerTransformCom->Get_State(STATE::POSITION) - m_pBoss->Get_Transform()->Get_State(STATE::POSITION);

        CFireBall::FIREBALL_DESC Desc;
        Desc.fSpeedPerSec = 20.f;
        Desc.IsEnemy = true;
        XMStoreFloat3(&Desc.vPosition, m_pBoss->Get_Transform()->Get_State(STATE::POSITION));
        XMStoreFloat3(&Desc.vLook, vLook);

        m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_FireBall"),
            m_pGameInstance->Get_LevelID(), TEXT("Layer_Skill"), &Desc);

        m_IsFireballThrow = true;
        m_iFireBallCount++;
    }

    if (true == m_IsFireballThrow)
    {
        m_fTimeAcc += fTimeDelta;
    
        if (0.25f <= m_fTimeAcc && m_iFireBallCount < 3)
        {
            _vector vLook = m_pPlayerTransformCom->Get_State(STATE::POSITION) - m_pBoss->Get_Transform()->Get_State(STATE::POSITION);

            CFireBall::FIREBALL_DESC Desc;
            Desc.fSpeedPerSec = 20.f;
            Desc.IsEnemy = true;
            XMStoreFloat3(&Desc.vPosition, m_pBoss->Get_Transform()->Get_State(STATE::POSITION));
            XMStoreFloat3(&Desc.vLook, vLook);

            m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_FireBall"),
                m_pGameInstance->Get_LevelID(), TEXT("Layer_Skill"), &Desc);

            m_iFireBallCount++;
            m_fTimeAcc = 0.f;
        }
    }

    if (true == IsAnimFinished && m_iFireBallCount >= 3)
    {
        _float fRandom = m_pGameInstance->Random_Normal();

        if(fRandom >= 0.5f)
            pNextState = CBoss_StepState::Create(m_pNavigationCom, m_pBoss, CBoss_StepState::ANIM_STATE::LEFT);
        else
            pNextState = CBoss_StepState::Create(m_pNavigationCom, m_pBoss, CBoss_StepState::ANIM_STATE::RIGHT);
    }
        //if (true == m_pBoss->Use_Skill())
        //{
        //    _float fRandom = m_pGameInstance->Random_Normal();

        //    if (fRandom > 0.5f)
        //        pNextState = CPajama_SlidingKunaiState::Create(m_pNavigationCom, m_pPajama, CPajama_SlidingKunaiState::ANIM_STATE::LEFT);
        //    else
        //        pNextState = CPajama_SlidingKunaiState::Create(m_pNavigationCom, m_pPajama, CPajama_SlidingKunaiState::ANIM_STATE::RIGHT);
        //}


    return pNextState;
}

_bool CBoss_FireBallState::End()
{
    return true;
}

CBoss_FireBallState* CBoss_FireBallState::Create(CNavigation* pNavigation, CBoss* pBoss)
{
    return new CBoss_FireBallState(pNavigation, pBoss);
}

void CBoss_FireBallState::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
