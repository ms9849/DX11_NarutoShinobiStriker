#include "Bird_ElectricShockState.h"

#include "Bird.h"
#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Bird_IdleState.h"

#pragma endregion

CBird_ElectricShockState::CBird_ElectricShockState(CNavigation* pNavigation, CBird* pBird, _vector vDirection, _float fRatio)
{
}

void CBird_ElectricShockState::Start(_bool IsBlend)
{
}

CBirdState* CBird_ElectricShockState::Update(_float fTimeDelta)
{
    return nullptr;
}

_bool CBird_ElectricShockState::End()
{
    return _bool();
}

CBird_ElectricShockState* CBird_ElectricShockState::Create(CNavigation* pNavigation, CBird* pBird, _vector vDirection, _float fRatio)
{
    return nullptr;
}

void CBird_ElectricShockState::Free()
{
}
