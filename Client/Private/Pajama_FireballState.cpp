#include "Pajama_FireballState.h"

#include "GameManager.h"
#include "GameInstance.h"

#include "Player.h"
#include "Pajama.h"
#include "FireBall.h"

#pragma region TRANSFER_STATE

#include "Pajama_IdleState.h"
#include "Pajama_SlidingKunaiState.h"

#pragma endregion

CPajama_FireballState::CPajama_FireballState(CNavigation* pNavigation, CPajama* pPajama)
	: m_pNavigationCom{ pNavigation }
	, m_pPajama{ pPajama }
{
	Safe_AddRef(m_pNavigationCom);
}

void CPajama_FireballState::Start(_bool IsBlend)
{
    m_pPajama->Get_Transform()->LookAt_XZ(m_pPlayerTransformCom->Get_State(STATE::POSITION));
	m_pPajama->Set_AnimIndex("CustomMan_Ninjutsu_Fireball_Lv3", 1.4f, IsBlend, 0.1f);

    m_pGameInstance->PlaySoundOnce(TEXT("Pajama_FireBall.wav"), CHANNELID::EFFECT, 0.45f);
}

CPajamaState* CPajama_FireballState::Update(_float fTimeDelta)
{
    CPajamaState* pNextState = { nullptr };

    _bool IsAnimFinished = m_pPajama->Play_Animation(fTimeDelta);
    _float fAnimProgress = m_pPajama->Get_AnimProgress();

    if (false == m_IsFireballThrow && fAnimProgress >= 0.7f)
    {
        _vector vLook = m_pPlayerTransformCom->Get_State(STATE::POSITION) - m_pPajama->Get_Transform()->Get_State(STATE::POSITION);

        CFireBall::FIREBALL_DESC Desc;
        Desc.fSpeedPerSec = 20.f;
        Desc.IsEnemy = true;
        XMStoreFloat3(&Desc.vPosition, m_pPajama->Get_Transform()->Get_State(STATE::POSITION));
        XMStoreFloat3(&Desc.vLook, vLook);

        m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_FireBall"),
            m_pGameInstance->Get_LevelID(), TEXT("Layer_Skill"), &Desc);

        m_IsFireballThrow = true;
    }

    if (true == IsAnimFinished)
    {
        if (true == m_pPajama->Use_Sliding())
        {
            _float fRandom = m_pGameInstance->Random_Normal();

            if (fRandom > 0.5f)
                pNextState = CPajama_SlidingKunaiState::Create(m_pNavigationCom, m_pPajama, CPajama_SlidingKunaiState::ANIM_STATE::LEFT);
            else
                pNextState = CPajama_SlidingKunaiState::Create(m_pNavigationCom, m_pPajama, CPajama_SlidingKunaiState::ANIM_STATE::RIGHT);
        }
        else
            pNextState = CPajama_IdleState::Create(m_pNavigationCom, m_pPajama);
    }


    return pNextState;
}

_bool CPajama_FireballState::End()
{
	return true;
}

CPajama_FireballState* CPajama_FireballState::Create(CNavigation* pNavigation, CPajama* pPajama)
{
	return new CPajama_FireballState(pNavigation, pPajama);
}

void CPajama_FireballState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
