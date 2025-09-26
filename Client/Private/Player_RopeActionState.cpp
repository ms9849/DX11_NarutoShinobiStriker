#include "Player_RopeActionState.h"

#include "Player.h"
#include "GameInstance.h"

/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_LandState.h"
#include "Player_FrontJumpState.h"

#pragma endregion

CPlayer_RopeActionState::CPlayer_RopeActionState(CPlayer* pPlayer, _vector vDir, _vector vPosition)
    : m_pPlayer{ pPlayer }
{
    XMStoreFloat3(&m_vTargetDir, vDir);
    XMStoreFloat3(&m_vTargetPos, vDir);
}

void CPlayer_RopeActionState::Start(_bool IsBlend)
{
}

CPlayerState* CPlayer_RopeActionState::Update(_float fTimeDelta)
{
    return nullptr;
}

_bool CPlayer_RopeActionState::End()
{
    return _bool();
}

CPlayer_RopeActionState* CPlayer_RopeActionState::Create(CPlayer* pPlayer, _vector vDir, _vector vPosition)
{
    return nullptr;
}

void CPlayer_RopeActionState::Free()
{
}
