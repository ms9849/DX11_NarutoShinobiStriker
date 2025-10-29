#include "Player_BigSharkState.h"

#include "Player.h"
#include "GameInstance.h"
#include "BigShark.h"
#include "GameManager.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_IdleState.h"

#pragma endregion


CPlayer_BigSharkState::CPlayer_BigSharkState(CPlayer* pPlayer)
	: m_pPlayer { pPlayer }
	, m_pGameManager { CGameManager::GetInstance() }
{
	Safe_AddRef(m_pPlayer);
	Safe_AddRef(m_pGameManager);
}

void CPlayer_BigSharkState::Start(_bool IsBlend)
{
	m_pPlayer->Set_Invincible(true);
	m_pGameManager->Change_Camera(static_cast<LEVEL>(m_pGameInstance->Get_LevelID()), TEXT("BigShark_Action_Camera"), m_pGameInstance->Get_PipeLine_InverseFloat4x4(D3DTS::VIEW));
	m_pPlayer->Set_AnimIndex("CustomMan_Ninjutsu_SuperSharkBomb", 1.5f, IsBlend, 0.1f);

	m_pGameInstance->PlaySoundOnce(TEXT("BigShark_Voice.wav"), CHANNELID::EFFECT, 0.7f);
}

CPlayerState* CPlayer_BigSharkState::Update(_float fTimeDelta)
{
	CPlayerState* pNextState = { nullptr };
	_bool IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);
	_float fAnimProgress = m_pPlayer->Get_AnimProgress();

	if (fAnimProgress > 0.65f && false == m_IsCamChanged)
	{
		m_pGameManager->Change_Camera(static_cast<LEVEL>(m_pGameInstance->Get_LevelID()), TEXT("Main_Camera"));
		m_IsCamChanged = true;
	}

	if (fAnimProgress > 0.8f && false == m_IsBigSharkThrow)
	{
		CBigShark::BIGSHARK_DESC Desc;

		Desc.fSpeedPerSec = 20.f;
		XMStoreFloat3(&Desc.vPosition, m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));

		_vector vPosition = {};
		CTransform* pTargetTransform = CGameManager::GetInstance()->Calc_Target(m_pPlayer->Get_Transform()->Get_State(STATE::POSITION));

		/* 타겟이 없을때만 플레이어가 바라보는 방향으로 날아가게끔 한다. */
		if (nullptr == pTargetTransform)
			vPosition = m_pPlayer->Get_Transform()->Get_State(STATE::LOOK);
		/* 타겟이 있다면 타겟 방향으로 날아가게끔 한다. */
		else
			vPosition = pTargetTransform->Get_State(STATE::POSITION) - m_pPlayer->Get_Transform()->Get_State(STATE::POSITION);

		XMStoreFloat3(&Desc.vLook, vPosition);

		m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_BigShark"),
			m_pGameInstance->Get_LevelID(), TEXT("Layer_Skill"), &Desc);

		m_IsBigSharkThrow = true;
	}

	if (true == IsAnimFinished)
		pNextState = CPlayer_IdleState::Create(m_pPlayer);

	return pNextState;
}

_bool CPlayer_BigSharkState::End()
{
	m_pPlayer->Set_Invincible(false);

	return true;
}

CPlayer_BigSharkState* CPlayer_BigSharkState::Create(CPlayer* pPlayer)
{
	return new CPlayer_BigSharkState(pPlayer);
}

void CPlayer_BigSharkState::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
	Safe_Release(m_pGameManager);
}
