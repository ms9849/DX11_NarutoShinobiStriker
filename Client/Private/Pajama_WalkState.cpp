#include "Pajama_WalkState.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Player.h"
#include "Pajama.h"

#pragma region TRANSFER_STATE

#include "Pajama_ThrowKunaiState.h"
#include "Pajama_SlidingKunaiState.h"
#include "Pajama_HandAttackState.h"
#include "Pajama_RunState.h"
#include "Pajama_FireballState.h"

#pragma endregion


CPajama_WalkState::CPajama_WalkState(CNavigation* pNavigation, CPajama* pPajama)
    : m_pNavigationCom{ pNavigation }
    , m_pPajama{ pPajama }
{
	Safe_AddRef(m_pNavigationCom);
}

void CPajama_WalkState::Start(_bool IsBlend)
{
	m_pPajama->Set_AnimIndex("CustomMan_Walk_Loop", 0.9f, IsBlend, 0.1f);
}

CPajamaState* CPajama_WalkState::Update(_float fTimeDelta)
{
    CPajamaState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pPajama->Play_Animation(fTimeDelta);
    _float fDist = XMVectorGetX(XMVector3Length(m_pPajama->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));

    if (true == m_pPajama->Use_Skill())
    {
        pNextState = CPajama_FireballState::Create(m_pNavigationCom, m_pPajama);
    }
    else if (fDist > 6.f)
    {
        pNextState = CPajama_RunState::Create(m_pNavigationCom, m_pPajama);
    }
    else if (true == m_pPajama->Use_Sliding() && fDist >= 2.f)
    {
        _float fRandom = m_pGameInstance->Random_Normal();

        if(fRandom > 0.5f)
            pNextState = CPajama_SlidingKunaiState::Create(m_pNavigationCom, m_pPajama, CPajama_SlidingKunaiState::ANIM_STATE::LEFT);
        else
            pNextState = CPajama_SlidingKunaiState::Create(m_pNavigationCom, m_pPajama, CPajama_SlidingKunaiState::ANIM_STATE::RIGHT);
    }
    else if (true == m_pPajama->Use_Kunai() && fDist >= 2.f)
    {
        pNextState = CPajama_ThrowKunaiState::Create(m_pNavigationCom, m_pPajama);
    }
    else if (fDist >= 1.3f)
    {
        m_pPajama->Get_Transform()->Go_Right(fTimeDelta * 0.1f, m_pNavigationCom);
        m_pPajama->Get_Transform()->Chase(m_pPlayerTransformCom->Get_State(STATE::POSITION), fTimeDelta * 0.05f, nullptr, fDist);
        m_pPajama->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
    }
    else if (fDist < 1.3f)
    {
        pNextState = CPajama_HandAttackState::Create(m_pNavigationCom, m_pPajama);
    }

    return pNextState;
}

_bool CPajama_WalkState::End()
{
	return true;
}

CPajama_WalkState* CPajama_WalkState::Create(CNavigation* pNavigation, CPajama* pPajama)
{
	return new CPajama_WalkState(pNavigation, pPajama);
}

void CPajama_WalkState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
