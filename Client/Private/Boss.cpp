#include "Boss.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Head_Character.h"
#include "Face_Character.h"
#include "Upper_Character.h"
#include "Lower_Character.h"
#include "Weapon_Character.h"

#include "Player.h"

#include "Boss_IdleState.h"
#include "Boss_BeatenState.h"
#include "Boss_BeatenBlastedState.h"
#include "Boss_ElectricShockState.h"
#include "Boss_DeadState.h"
#include "Boss_StandState.h"

#include "BossHPPanel.h"
#include "Icon.h"
#include "ParticleObject.h"
#include "Trail.h"

CBoss::CBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CEnemy{ pDevice, pContext, eObjectID }
{
}

CBoss::CBoss(const CBoss& rhs)
    : CEnemy{ rhs }
{
}

#pragma region SETTER

_float CBoss::Get_AnimProgress()
{
    return dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Upper")))->Get_AnimProgress();
}

void CBoss::Set_AnimProgress(_float fProgress)
{
    for (auto& pPart : m_PartObjects)
    {
        dynamic_cast<CParts_Character*>(pPart.second)->Set_AnimProgress(fProgress);
    }
}

void CBoss::Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate, _bool IsBlend, _float fBlendRatio, _bool IsLoop)
{
    for (auto& pPart : m_PartObjects)
    {
        dynamic_cast<CParts_Character*>(pPart.second)->Set_AnimIndex(pAnimName, fAnimationPlayRate, IsBlend, fBlendRatio, IsLoop);
    }
}

_bool CBoss::Play_Animation(_float fTimeDelta)
{
    _bool isAnimFinished = { false };

    isAnimFinished = dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Upper")))->Play_Animation(fTimeDelta);

    dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Face")))->Play_Animation(fTimeDelta);
    dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Head")))->Play_Animation(fTimeDelta);

    return isAnimFinished;
}

void CBoss::Set_Collider_Active(const _wstring& strColliderTag, _bool bFlag)
{
    static_cast<CCollider*>(Find_Component(strColliderTag))->Set_Active(bFlag);
}

CCollider* CBoss::Get_Collider(const _wstring& strColliderTag)
{
    return static_cast<CCollider*>(Find_Component(strColliderTag));
}

#pragma endregion

void CBoss::OnCollision(COLLIDER_HANDLE_ID eHandleID)
{
    if (true == m_IsInvincible || true == m_IsPlayingDeadAnim)
        return;

    m_pGameManager->Active_Combo();

    CBossState* pNextState = { nullptr };

    _vector vDirection = m_pTransformCom->Get_State(STATE::POSITION) -
        CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform()->Get_State(STATE::POSITION);


    _float fHitTime = { 0.3f };

    if (COLLIDER_HANDLE_ID::PLAYER_HAND_ATTACK == eHandleID)
    {
        CParticleObject::PARTICLE_LOAD_DESC Desc;
        Desc.strParticlePath = TEXT("../Bin/Resources/Particle/Player_Attack_Particle.bin");

        m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"), m_pGameInstance->Get_LevelID(),
            TEXT("Layer_Particle"), &Desc);

        m_fCurrentHP -= 1.f;
        pNextState = CBoss_BeatenState::Create(m_pNavigationCom, this, vDirection, 1.5f);
    }
    else if (COLLIDER_HANDLE_ID::PLAYER_HAND_ATTACK_FINAL == eHandleID)
    {
        CParticleObject::PARTICLE_LOAD_DESC Desc;
        Desc.strParticlePath = TEXT("../Bin/Resources/Particle/Player_Attack_Particle.bin");

        m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ParticleObject"), m_pGameInstance->Get_LevelID(),
            TEXT("Layer_Particle"), &Desc);

        m_fCurrentHP -= 3.f;

        pNextState = CBoss_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection, 1.f);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_SWORD_ATTACK == eHandleID)
    {
        m_fCurrentHP -= 3.f;

        pNextState = CBoss_BeatenState::Create(m_pNavigationCom, this, vDirection, 1.75f);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_SWORD_ATTACK_FINAL == eHandleID)
    {
        m_fCurrentHP -= 3.f;

        pNextState = CBoss_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection, 1.f);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENGAN == eHandleID ||
        COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_FIREBALL == eHandleID)
    {
        m_fCurrentHP -= 15.f;

        pNextState = CBoss_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection, 1.f);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENSHURIKEN == eHandleID ||
        COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENSHURIKEN_EXPLODE == eHandleID)
    {
        m_fCurrentHP -= 1.f;

        pNextState = CBoss_BeatenState::Create(m_pNavigationCom, this, vDirection, 0.f);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_KAMUI == eHandleID)
    {
        m_fCurrentHP -= 1.f;

        pNextState = CBoss_BeatenState::Create(m_pNavigationCom, this, vDirection, 0.f);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_KAMUI_END == eHandleID ||
        COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENSHURIKEN_END == eHandleID)
    {
        m_fCurrentHP -= 5.f;

        pNextState = CBoss_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection, 1.f);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_BIGSHARK == eHandleID)
    {
        m_fCurrentHP -= 15.f;

        pNextState = CBoss_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection, 1.f);
        Set_Invincible(1.f);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_CHIDORI == eHandleID)
    {
        m_pGameManager->Change_Camera(static_cast<LEVEL>(m_pGameInstance->Get_LevelID()),
            TEXT("Chidori_Action_Camera"), nullptr);


        fHitTime = 2.f;
        m_fCurrentHP -= 15.f;
        pNextState = CBoss_ElectricShockState::Create(m_pNavigationCom, this);
        Set_Invincible(1.f);
    }

    if (m_fCurrentHP <= 0.f && false == m_IsPlayingDeadAnim)
    {
        m_IsPlayingDeadAnim = true;
        if (nullptr != pNextState)
        {
            pNextState->End();
            Safe_Release(pNextState);
        }
        pNextState = CBoss_DeadState::Create(m_pNavigationCom, this);
        m_pHPBar->Set_Dead(true);
        m_pIcon->Set_Dead(true);
    }

    for (auto& iter : m_PartObjects)
        static_cast<CParts_Character*>(iter.second)->Set_HitEffect(fHitTime, 1.f);

    Change_State(pNextState, false);

    m_fCurrentHP -= 1.f;
}

void CBoss::Change_State(CBossState* pNextState, _bool bBlend)
{
    _bool IsBlend = m_pState->End();
    Safe_Release(m_pState);

    pNextState->Start(bBlend);
    m_pState = pNextState;
}

HRESULT CBoss::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBoss::Initialize(void* pArg)
{
    CGameObject::GAMEOBJECT_DESC	Desc{};
    Desc.fRotationPerSec = XMConvertToRadians(180.0f);
    Desc.fSpeedPerSec = 10.f;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Position()))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    /* 보스는 전용 체력바 사용함. */
    //if (FAILED(Ready_HPBar()))
    //    return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_pGameManager->Add_TargetTransform(m_pTransformCom);

    /* 상태 초기화 및 시작. */
    m_pState = CBoss_StandState::Create(m_pNavigationCom, this);
    m_pState->Start(true);

    m_SkillCoolDowns[ENUM_CLASS(BOSS_SKILL::FIREBALL)] = 10.f;
    m_SkillCoolDowns[ENUM_CLASS(BOSS_SKILL::LIGHTING_RUSH)] = 15.f;
    m_SkillCoolDowns[ENUM_CLASS(BOSS_SKILL::SHARINGAN)] = 600.f;
    m_SkillCoolDowns[ENUM_CLASS(BOSS_SKILL::SPIN_KICK)] = 5.f;
    m_SkillCoolDowns[ENUM_CLASS(BOSS_SKILL::WOODHAND)] = 10.f;

    /* 이 둘은 바로 쓸 수 있게 세팅. */
    m_SkillTimeAccs[ENUM_CLASS(BOSS_SKILL::SHARINGAN)] = 600.f;
    m_SkillTimeAccs[ENUM_CLASS(BOSS_SKILL::LIGHTING_RUSH)] = 15.f;

    m_fCurrentHP = 200.f;
    m_fMaxHP = 200.f;

    Fade_Particle();

    CTrail::TRAIL_DESC TrailDesc;
    TrailDesc.vFootTrailColor = _float4(1.0f, 0.6f, 0.2f, 1.f);
    TrailDesc.strTrailTextureTag = TEXT("Prototype_Component_Texture_FootTrail_Blue");
    XMStoreFloat4(&TrailDesc.vHighPosition, XMVectorSet(-0.02f, 0.f, 0.f, 1.f));
    XMStoreFloat4(&TrailDesc.vLowPosition, XMVectorSet(0.f, 0.f, 0.02f, 1.f));

    m_pFootTrail[0] = static_cast<CTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_FootTrail"), &TrailDesc));
    m_pFootTrail[1] = static_cast<CTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_FootTrail"), &TrailDesc));

    return S_OK;
}

void CBoss::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CBoss::Update(_float fTimeDelta)
{
    if (false == m_IsActive)
        return;

    Update_State(fTimeDelta);
    Update_SkillCoolDown(fTimeDelta);
    Update_FootTrail(fTimeDelta);

    if (false == m_IsFlying)
        m_pNavigationCom->Compute_Height(m_pTransformCom);

    __super::Update(fTimeDelta);

    m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    _matrix PlayerMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    _vector PlayerTranslation = m_pTransformCom->Get_State(STATE::POSITION);
    PlayerTranslation += m_pTransformCom->Get_State(STATE::LOOK) * 0.4f;
    PlayerMatrix.r[3] = PlayerTranslation;

    m_pHandAttackColliderCom->Update(PlayerMatrix);
    m_pSpinKickColliderCom->Update(PlayerMatrix);
    m_pRushColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
    /* 체력 설정. 한프레임 늦긴한다. */
    if(nullptr != m_pHPBar)
        m_pHPBar->Set_Progress(m_fCurrentHP);

    if (nullptr != m_pIcon)
        m_pIcon->Set_Position(m_pTransformCom->Get_State(STATE::POSITION) + XMVectorSet(0.f, 2.0f, 0.f, 0.f));
}

void CBoss::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    /* 몸통 콜라이더를 콜리전 매니저에 등록 */
    m_pGameManager->Add_Object_ToCollision(TEXT("Monster_Body"), this, m_pColliderCom);
    m_pGameManager->Add_Collider_ToCollision(TEXT("Monster_Attack"), COLLIDER_HANDLE_ID::ENEMY_JETSU_ATTACK, m_pHandAttackColliderCom);
    m_pGameManager->Add_Collider_ToCollision(TEXT("Monster_Attack"), COLLIDER_HANDLE_ID::ENEMY_BOXER_SPINKICK, m_pSpinKickColliderCom);
    m_pGameManager->Add_Collider_ToCollision(TEXT("Monster_Attack"), COLLIDER_HANDLE_ID::ENEMY_BOXER_SPINKICK, m_pSpinKickColliderCom);


    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
    m_pGameInstance->Add_DebugComponent(m_pHandAttackColliderCom);
    m_pGameInstance->Add_DebugComponent(m_pSpinKickColliderCom);
    m_pGameInstance->Add_DebugComponent(m_pRushColliderCom);
#endif

    LateUpdate_FootTrail(fTimeDelta);
}

HRESULT CBoss::Render()
{
    return S_OK;
}

_bool CBoss::Use_Skill(BOSS_SKILL eSkillList)
{
    if(m_SkillTimeAccs[ENUM_CLASS(eSkillList)] >= m_SkillCoolDowns[ENUM_CLASS(eSkillList)])
    {
        /* 쿨타임 초기화*/
        m_SkillTimeAccs[ENUM_CLASS(eSkillList)] = 0.f;

        return true;
    }

    return false;
}

void CBoss::Update_SkillCoolDown(_float fTimeDelta)
{
    /* 쿨타임이 존재하는 여러 동작들에 대해 연산을 수행해줌. */

    for (_uint i = 0; i < ENUM_CLASS(BOSS_SKILL::END); ++i)
    {
        m_SkillTimeAccs[i] += fTimeDelta;

        if (m_SkillTimeAccs[i] >= m_SkillCoolDowns[i])
            m_SkillTimeAccs[i] = m_SkillCoolDowns[i];
    }
}

HRESULT CBoss::Start_Battle()
{
    if (FAILED(Ready_BossHPPanel()))
        return E_FAIL;

    if (FAILED(Ready_BossIcon()))
        return E_FAIL;

    Change_State(CBoss_IdleState::Create(m_pNavigationCom, this), true);

    return S_OK;
}

HRESULT CBoss::Ready_Position()
{
    if (TRIGGER_TYPE::TUTORIAL_SPAWNER_01 == m_pGameManager->Get_CurrentTrigger())
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

    else if (TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_BOSS == m_pGameManager->Get_CurrentTrigger())
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-85.641f, 28.7f, 59.220f, 1.f));
        m_pTransformCom->Rotation(0.f, 180.f, 0.f);
    }

    return S_OK;
}

HRESULT CBoss::Ready_Components()
{
    /* Com_Navigation */
    CNavigation::NAVIGATION_DESC Desc;
    Desc.iCurrentCellIndex = 0;
    XMStoreFloat3(&Desc.vPosition, m_pTransformCom->Get_State(STATE::POSITION));

    if (LEVEL::TUTORIAL == m_pGameManager->Get_NextLevel())
    {
        if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_Component_Navigation_Tutorial"),
            TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
            return E_FAIL;
    }
    else if (LEVEL::KONOHA_VILLAGE == m_pGameManager->Get_NextLevel())
    {
        if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Prototype_Component_Navigation_KonohaVillage_4"),
            TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
            return E_FAIL;
    }

    /* Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC OBBDesc{};

    OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
    OBBDesc.vSize = _float3(0.7f, 1.4f, 0.7f);
    OBBDesc.vCenter = _float3(0.f, 0.8f, 0.f);
    OBBDesc.isActive = true;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
        return E_FAIL;

    CBounding_Sphere::BOUNDING_SPHERE_DESC ColliderDesc{};

    ColliderDesc.fRadius = 0.7f;
    ColliderDesc.vCenter = _float3{ 0.f, 0.7f, 0.f };
    ColliderDesc.isActive = false;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_HandAttack"), reinterpret_cast<CComponent**>(&m_pHandAttackColliderCom), &ColliderDesc)))
        return E_FAIL;


    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_SpinKick"), reinterpret_cast<CComponent**>(&m_pSpinKickColliderCom), &ColliderDesc)))
        return E_FAIL;

    ColliderDesc.fRadius = 1.4f;
    ColliderDesc.vCenter = _float3{ 0.f, 1.4f, 0.f };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_Rush"), reinterpret_cast<CComponent**>(&m_pRushColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBoss::Ready_PartObjects()
{
    CUpper_Character::UPPER_PLAYER_DESC UpperDesc{};
    UpperDesc.pParentTransform = m_pTransformCom;
    UpperDesc.strModelName = TEXT("Prototype_Component_Model_Upper_Boss");

    CHead_Character::HEAD_PLAYER_DESC HeadDesc{};
    HeadDesc.pParentTransform = m_pTransformCom;
    HeadDesc.strModelName = TEXT("Prototype_Component_Model_Head_Boss");

    CFace_Character::tagFace_Player_Desc FaceDesc{};
    FaceDesc.pParentTransform = m_pTransformCom;
    FaceDesc.strModelName = TEXT("Prototype_Component_Model_Face_Boss");

    /* Part_Upper */
    if (FAILED(__super::Add_PartObject(m_pGameInstance->Get_LevelID(), TEXT("Prototype_GameObject_Upper_Player"),
        TEXT("Part_Upper"), &UpperDesc)))
        return E_FAIL;

    /* Part_Head */
    if (FAILED(__super::Add_PartObject(m_pGameInstance->Get_LevelID(), TEXT("Prototype_GameObject_Head_Player"),
        TEXT("Part_Head"), &HeadDesc)))
        return E_FAIL;

    /* Part_Face */
    if (FAILED(__super::Add_PartObject(m_pGameInstance->Get_LevelID(), TEXT("Prototype_GameObject_Face_Player"),
        TEXT("Part_Face"), &FaceDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBoss::Ready_BossHPPanel()
{
    CUIObject::UIOBJECT_DESC Desc;
    Desc.fSizeX = 400.f;
    Desc.fSizeY = 100.f;
    Desc.fX = g_iWinSizeX / 2.f + 10.f;
    Desc.fY = g_iWinSizeY / 2.f - 270.f;
    Desc.fZ = 0.1f;

    /* 추후 레벨 수정 */
    m_pHPBar = static_cast<CBossHPPanel*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_pGameInstance->Get_LevelID(),
        TEXT("Prototype_GameObject_BossHPPanel"), &Desc));

    if (nullptr == m_pHPBar)
        return E_FAIL;

    Safe_AddRef(m_pHPBar);
    m_pGameInstance->Add_Clone_ToLayer(m_pHPBar, m_pGameInstance->Get_LevelID(), TEXT("Layer_UI"));

    /* 내 최대 HP로 세팅. */
    m_pHPBar->Set_MaxProgress(m_fMaxHP);

    return S_OK;
}

HRESULT CBoss::Ready_BossIcon()
{
    CIcon::ICON_DESC Desc;
    Desc.iTextureIdx = 0;

    m_pIcon = static_cast<CIcon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_pGameInstance->Get_LevelID(),
        TEXT("Prototype_GameObject_Icon"), &Desc));

    Safe_AddRef(m_pIcon);
    m_pGameInstance->Add_Clone_ToLayer(m_pIcon, m_pGameInstance->Get_LevelID(), TEXT("Layer_UI"));

    return S_OK;
}

void CBoss::Update_State(_float fTimeDelta)
{
    CBossState* pNextState = { nullptr };
    pNextState = m_pState->Update(fTimeDelta);

    if (nullptr != pNextState)
    {
        _bool IsBlend = m_pState->End();
        //현재 스테이트 날려버림.
        Safe_Release(m_pState);

        //새로운 상태 시작해줌.
        pNextState->Start(IsBlend);

        m_pState = pNextState;
    }
}

CBoss* CBoss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CBoss* pInstance = new CBoss(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : Boss");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CBoss::Clone(void* pArg)
{
    CBoss* pInstance = new CBoss(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : Boss");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBoss::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pHandAttackColliderCom);
    Safe_Release(m_pSpinKickColliderCom);
    Safe_Release(m_pRushColliderCom);
    Safe_Release(m_pState);
    Safe_Release(m_pIcon);
    Safe_Release(m_pHPBar);
}
