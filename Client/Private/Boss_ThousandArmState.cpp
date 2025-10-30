#include "Boss_ThousandArmState.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Boss.h"

#pragma region TRANSFER_STATE

#include "Boss_IdleState.h"

#pragma endregion

#include "ThousandArm.h"
#include "Player.h"

CBoss_ThousandArmState::CBoss_ThousandArmState(CNavigation* pNavigation, CBoss* pBoss)
	: m_pNavigationCom{ pNavigation }
	, m_pBoss{ pBoss }
{
	Safe_AddRef(m_pNavigationCom);
}

void CBoss_ThousandArmState::Start(_bool IsBlend)
{
	/* 여기서 카메라 보스로 변경 */
	m_pBoss->Set_AnimIndex("CustomMan_Bee_HandSeal_RecoveryChakra_Start", 0.5f, true);

	CGameManager::GetInstance()->Change_Camera(LEVEL::KONOHA_VILLAGE, TEXT("Boss_Camera"), nullptr);
	CGameManager::GetInstance()->Get_PlayerPtr()->Set_Visible(false);

	m_pGameInstance->PlaySoundOnce(TEXT("Boss_ThousandHand.wav"), CHANNELID::EFFECT2, 0.45f);
}

CBossState* CBoss_ThousandArmState::Update(_float fTimeDelta)
{
	CBossState* pNextState = { nullptr };
	_float fAnimProgress = m_pBoss->Get_AnimProgress();
	_bool IsAnimFinished = m_pBoss->Play_Animation(fTimeDelta);


	m_fTimeAcc += fTimeDelta;

	if (true == IsAnimFinished)
	{
		m_pBoss->Set_AnimIndex("CustomMan_Bee_HandSeal_RecoveryChakra_Start", 0.5f, false);
		m_pBoss->Set_AnimProgress(1.0f);

		if(m_fTimeAcc >= 3.6f)
			pNextState = CBoss_IdleState::Create(m_pNavigationCom, m_pBoss);
		
		/* 3.5초 안지낫으면*/
		else if (m_fTimeAcc > 1.5f && false == m_IsButdaCamOn)
		{
			_float4x4 CameraMatrix = {
				0.321f, 0.f, 0.947f, 0.f,
				0.506f, 0.845f, -0.172f, 0.f,
				-0.8f, 0.534f, 0.272f, 0.f,
				-118.416f, 69.621f, 27.504f, 1.f
			};
			/* 여기서 카메라 천수 바라보게 변경 */
			static_cast<CThousandArm*>(m_pGameInstance->Get_GameObject(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Layer_ThousandArm"), 0))->Set_Visible(true);
			CGameManager::GetInstance()->Change_Camera(LEVEL::KONOHA_VILLAGE, TEXT("ThousandHand_Camera"), &CameraMatrix);

			m_IsButdaCamOn = true;
		}
	}


	return pNextState;
}

_bool CBoss_ThousandArmState::End()
{
	return true;
}

CBoss_ThousandArmState* CBoss_ThousandArmState::Create(CNavigation* pNavigation, CBoss* pBoss)
{
	return new CBoss_ThousandArmState(pNavigation, pBoss);
}

void CBoss_ThousandArmState::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
}
