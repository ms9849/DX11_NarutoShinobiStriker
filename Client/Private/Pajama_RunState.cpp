#include "Pajama_RunState.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Player.h"
#include "Pajama.h"

#pragma region TRANSFER_STATE

#include "Pajama_ThrowKunaiState.h"
#include "Pajama_SlidingKunaiState.h"
#include "Pajama_WalkState.h"
#include "Pajama_FireballState.h"

#pragma endregion

CPajama_RunState::CPajama_RunState(CNavigation* pNavigation, CPajama* pPajama)
	: m_pNavigationCom{ pNavigation }
	, m_pPajama{ pPajama }
{
	Safe_AddRef(m_pNavigationCom);
}

void CPajama_RunState::Start(_bool IsBlend)
{
	m_pPajama->Set_AnimIndex("CustomMan_Run_Loop", 0.8f, IsBlend, 0.1f);
}

CPajamaState* CPajama_RunState::Update(_float fTimeDelta)
{
    CPajamaState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pPajama->Play_Animation(fTimeDelta);
    _float fDist = XMVectorGetX(XMVector3Length(m_pPajama->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));

    /* 추후 슬라이딩 쿠나이로 구현할 것 */

    if (true == m_pPajama->Use_Skill())
    {
        pNextState = CPajama_FireballState::Create(m_pNavigationCom, m_pPajama);
    }
    else if (true == m_pPajama->Use_Sliding() && fDist > 5.f)
    {
        _float fRandom = m_pGameInstance->Random_Normal();

        if (fRandom > 0.5f)
            pNextState = CPajama_SlidingKunaiState::Create(m_pNavigationCom, m_pPajama, CPajama_SlidingKunaiState::ANIM_STATE::LEFT);
        else
            pNextState = CPajama_SlidingKunaiState::Create(m_pNavigationCom, m_pPajama, CPajama_SlidingKunaiState::ANIM_STATE::RIGHT);
    }
    else if (fDist > 5.f)
    {
        m_pPajama->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
        m_pPajama->Get_Transform()->Chase_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION), fTimeDelta, m_pNavigationCom, 1.f);
    }

    else if (fDist <= 5.f)
    {
        pNextState = CPajama_WalkState::Create(m_pNavigationCom, m_pPajama);
    }

    return pNextState;
}

_bool CPajama_RunState::End()
{
	return true;
}

CPajama_RunState* CPajama_RunState::Create(CNavigation* pNavigation, CPajama* pPajama)
{
	return new CPajama_RunState(pNavigation, pPajama);
}

void CPajama_RunState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
