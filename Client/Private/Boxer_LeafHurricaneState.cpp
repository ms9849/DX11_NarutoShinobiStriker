#include "Boxer_LeafHurricaneState.h"

#include "GameManager.h"
#include "GameInstance.h"
#include "Player.h"
#include "Boxer.h"

#pragma region TRANSFER_STATE

#include "Boxer_RunState.h"
#include "Boxer_IdleState.h"

#pragma endregion

CBoxer_LeafHurricaneState::CBoxer_LeafHurricaneState(CNavigation* pNavigation, CBoxer* pBoxer)
	: m_pNavigationCom{ pNavigation }
	, m_pBoxer{ pBoxer }
{
	Safe_AddRef(m_pNavigationCom);
}

void CBoxer_LeafHurricaneState::Start(_bool IsBlend)
{
	m_pBoxer->Set_AnimIndex("CustomMan_Ninjutsu_LeafHurricane_Start", 1.f, IsBlend, 0.1f, false);
	m_pPlayerTransformCom = CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform();
	Safe_AddRef(m_pPlayerTransformCom);
	m_eAnimState = ANIM_STATE::ATTACK_START;
}

CBoxerState* CBoxer_LeafHurricaneState::Update(_float fTimeDelta)
{
	CBoxerState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pBoxer->Play_Animation(fTimeDelta);

	_float fDist = XMVectorGetX(XMVector3Length(m_pBoxer->Get_Transform()->Get_State(STATE::POSITION) - m_pPlayerTransformCom->Get_State(STATE::POSITION)));

	if (ANIM_STATE::ATTACK_START == m_eAnimState)
	{
		m_pBoxer->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
		m_pBoxer->Get_Transform()->Chase_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION), fTimeDelta * 2.f);
	}


	/* ³ª¹µÀÙ ¼±Ç³ ½ÃÀÛ »óÅÂ¿¡´Ù°¡, ´Þ·Á°¡´Â Áß¿¡ ÇÃ·¹ÀÌ¾î¶û °¡±õ´Ù¸é */
	if (ANIM_STATE::ATTACK_START == m_eAnimState
		&& (true == IsAnimFinished || fDist <= 2.f) )
	{
		m_pBoxer->Set_AnimIndex("CustomMan_Ninjutsu_LeafHurricane", 1.f, false, 0.1f, true);
		m_eAnimState = ANIM_STATE::ATTACK_END;
	}
	/* ³ª¹µÀÙ ¼±Ç³ ´Þ·Á°¡´Â Áß*/
	else if (ANIM_STATE::ATTACK_START == m_eAnimState
		&& true == IsAnimFinished)
	{
		m_pBoxer->Set_AnimIndex("CustomMan_Ninjutsu_LeafHurricane_Start", 1.f, false, 0.1f, false);
		m_pBoxer->Set_AnimProgress(1.0f);
	}
	else if (ANIM_STATE::ATTACK_END == m_eAnimState
		&& true == IsAnimFinished)
	{
		pNextState = CBoxer_IdleState::Create(m_pNavigationCom, m_pBoxer);
	}

    return pNextState;
}

_bool CBoxer_LeafHurricaneState::End()
{
    return true;
}

CBoxer_LeafHurricaneState* CBoxer_LeafHurricaneState::Create(CNavigation* pNavigation, CBoxer* pBoxer)
{
    return new CBoxer_LeafHurricaneState(pNavigation, pBoxer);
}

void CBoxer_LeafHurricaneState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pPlayerTransformCom);
}
