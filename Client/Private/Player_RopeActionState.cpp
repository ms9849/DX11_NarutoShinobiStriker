#include "Player_RopeActionState.h"

#include "Player.h"
#include "GameInstance.h"
#include "Rope.h"
/* 전이 가능한 상태들 */
#pragma region TRANSFER_STATE

#include "Player_LandState.h"
#include "Player_FrontJumpState.h"

#pragma endregion

CPlayer_RopeActionState::CPlayer_RopeActionState(CPlayer* pPlayer)
    : m_pPlayer{ pPlayer }
{
}

void CPlayer_RopeActionState::Start(_bool IsBlend)
{
    m_pPlayer->Set_Ground(false);
    m_pPlayer->Set_AnimIndex("CustomMan_WireJump_Ready", 2.f, IsBlend);
    m_eAnimState = ANIM_STATE::THROW;

    /* 피킹 로직. Dist는 당장 사용하지 않음. */
    _float fDist;
    _matrix HandMatrix = XMLoadFloat4x4(m_pPlayer->Get_BoneMatrix(TEXT("Part_Upper"), "RightHandMiddle1"));
    m_pGameInstance->Check_Ray_GeometryPicking(m_pGameInstance->Get_RayPos(RAY::CENTER), m_pGameInstance->Get_RayDir(RAY::CENTER), &m_vTargetPos, &fDist);
    XMStoreFloat3(&m_vStartPos, (HandMatrix * XMLoadFloat4x4(m_pPlayer->Get_Transform()->Get_WorldMatrixPtr())).r[3]);

    //m_vTargetDir
    XMStoreFloat3(&m_vTargetDir, XMVector3Normalize(XMLoadFloat3(&m_vTargetPos) - XMLoadFloat3(&m_vStartPos)));


    m_pPlayer->Get_Transform()->LookAt_XZ(XMLoadFloat3(&m_vTargetPos));
}

CPlayerState* CPlayer_RopeActionState::Update(_float fTimeDelta)
{
    CPlayerState* pNextState = { nullptr };
    _bool IsAnimFinished = { false };

    if (ANIM_STATE::WAIT != m_eAnimState)
       IsAnimFinished = m_pPlayer->Play_Animation(fTimeDelta);

    if (true == IsAnimFinished && m_eAnimState == ANIM_STATE::THROW)
    {
        /* 로프 생성 & 애니메이션 변경 */
        CRope::ROPE_DESC Desc;
        Desc.vStartPos = m_vStartPos;
        Desc.vTargetPos = m_vTargetPos;

        m_eAnimState = ANIM_STATE::WAIT;
        
        m_pRope = static_cast<CRope*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC),
            TEXT("Prototype_GameObject_Rope"), &Desc));
        Safe_AddRef(m_pRope);

        m_pGameInstance->Add_Clone_ToLayer(m_pRope, m_pGameInstance->Get_LevelID(), TEXT("Layer_Rope"));
    }

    if (m_eAnimState == ANIM_STATE::WAIT && true == m_pRope->IsArrive())
    {
        m_pPlayer->Set_AnimIndex("CustomMan_Aerial_Dash_Loop", 2.f, true);
        m_pPlayer->Set_Pickable(false);
        m_eAnimState = ANIM_STATE::ROPE;
    }

    /* 
    여기서 Check Geometry Collision 수행해야됨.
    IsGround가 True라면, 착지모션 세팅.
    만약 그렇지 않다면 충돌 체크 한 뒤 fall Front 모션 세팅.
    */
    if (m_eAnimState == ANIM_STATE::ROPE)
    {
        _bool IsCollision = { false };
        _bool IsGround = { false };
        m_pPlayer->Get_Transform()->Go_Direction(XMLoadFloat3(&m_vTargetDir), fTimeDelta * 3.f);
    
        IsGround = m_pGameInstance->Check_GameObject_GeometryCollision(m_pPlayer, &IsCollision);

        if (true == IsCollision && true == IsGround)
        {
            pNextState = CPlayer_LandState::Create(m_pPlayer);
            m_pPlayer->Set_Pickable(true);
            m_pRope->Set_Dead(true);
        }
        else if (true == IsCollision && false == IsGround)
        {
            pNextState = CPlayer_FrontJumpState::Create(m_pPlayer, 1.1f, CPlayer_FrontJumpState::ANIM_STATE::FALL);
            m_pPlayer->Set_Pickable(true);
            m_pRope->Set_Dead(true);
        }
    }

    return pNextState;
}

_bool CPlayer_RopeActionState::End()
{
    m_pPlayer->Get_Transform()->Set_State(STATE::UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
    m_pPlayer->Get_Transform()->Set_State(STATE::LOOK, XMVector3Cross(m_pPlayer->Get_Transform()->Get_State(STATE::RIGHT), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
    m_pPlayer->Set_Pickable(true);
    m_pPlayer->Set_Ground(true);

    return true;
}

CPlayer_RopeActionState* CPlayer_RopeActionState::Create(CPlayer* pPlayer)
{
    return new CPlayer_RopeActionState(pPlayer);
}

void CPlayer_RopeActionState::Free()
{
    __super::Free();

    Safe_Release(m_pRope);
}
