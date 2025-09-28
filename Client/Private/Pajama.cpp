#include "Pajama.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Head_Character.h"
#include "Face_Character.h"
#include "Upper_Character.h"
#include "Lower_Character.h"
#include "Weapon_Character.h"

#include "Player.h"
#include "Pajama_IdleState.h"

CPajama::CPajama(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CEnemy{ pDevice, pContext, eObjectID }
{
}

CPajama::CPajama(const CPajama& rhs)
    : CEnemy{ rhs }
{
}

_float CPajama::Get_AnimProgress()
{
    return dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Upper")))->Get_AnimProgress();
}

void CPajama::Set_AnimProgress(_float fProgress)
{
    for (auto& pPart : m_PartObjects)
    {
        dynamic_cast<CParts_Character*>(pPart.second)->Set_AnimProgress(fProgress);
    }
}

void CPajama::Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate, _bool IsBlend, _float fBlendRatio, _bool IsLoop)
{
    for (auto& pPart : m_PartObjects)
    {
        dynamic_cast<CParts_Character*>(pPart.second)->Set_AnimIndex(pAnimName, fAnimationPlayRate, IsBlend, fBlendRatio, IsLoop);
    }
}

_bool CPajama::Play_Animation(_float fTimeDelta)
{
    _bool isAnimFinished = { false };

    isAnimFinished = dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Upper")))->Play_Animation(fTimeDelta);

    dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Face")))->Play_Animation(fTimeDelta);
    dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Head")))->Play_Animation(fTimeDelta);

    return isAnimFinished;
}

void CPajama::Set_Collider_Active(const _wstring& strColliderTag, _bool bFlag)
{
    static_cast<CCollider*>(Find_Component(strColliderTag))->Set_Active(bFlag);
}

CCollider* CPajama::Get_Collider(const _wstring& strColliderTag)
{
    return static_cast<CCollider*>(Find_Component(strColliderTag));
}

void CPajama::OnCollision(COLLIDER_HANDLE_ID eHandleID)
{
    /*if (true == m_IsInvincible || true == m_isPlayingDeadAnim)
        return;

    m_pGameManager->Active_Combo();

    CWhiteJetsuState* pNextState = { nullptr };

    _vector vDirection = m_pTransformCom->Get_State(STATE::POSITION) -
        CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform()->Get_State(STATE::POSITION);

    if (COLLIDER_HANDLE_ID::PLAYER_HAND_ATTACK == eHandleID)
    {
        m_fCurrentHP -= 1.f;

        pNextState = CWhiteJetsu_BeatenState::Create(m_pNavigationCom, this, vDirection, 1.5f);
    }
    else if (COLLIDER_HANDLE_ID::PLAYER_HAND_ATTACK_FINAL == eHandleID)
    {
        m_fCurrentHP -= 3.f;

        pNextState = CWhiteJetsu_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_SWORD_ATTACK == eHandleID)
    {
        m_fCurrentHP -= 3.f;

        pNextState = CWhiteJetsu_BeatenState::Create(m_pNavigationCom, this, vDirection, 1.75f);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_SWORD_ATTACK_FINAL == eHandleID)
    {
        m_fCurrentHP -= 3.f;

        pNextState = CWhiteJetsu_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENGAN == eHandleID ||
        COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_FIREBALL == eHandleID)
    {
        m_fCurrentHP -= 15.f;

        pNextState = CWhiteJetsu_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENSHURIKEN == eHandleID ||
        COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENSHURIKEN_EXPLODE == eHandleID)
    {
        m_fCurrentHP -= 1.f;

        pNextState = CWhiteJetsu_BeatenState::Create(m_pNavigationCom, this, vDirection, 0.f);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_KAMUI == eHandleID)
    {
        m_fCurrentHP -= 1.f;

        pNextState = CWhiteJetsu_BeatenState::Create(m_pNavigationCom, this, vDirection, 0.f);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_KAMUI_END == eHandleID ||
        COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_RASENSHURIKEN_END == eHandleID)
    {
        m_fCurrentHP -= 5.f;

        pNextState = CWhiteJetsu_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_BIGSHARK == eHandleID)
    {
        m_fCurrentHP -= 15.f;

        pNextState = CWhiteJetsu_BeatenBlastedState::Create(m_pNavigationCom, this, vDirection);
        Set_Invincible(1.f);
    }

    else if (COLLIDER_HANDLE_ID::PLAYER_NINJUTSU_CHIDORI == eHandleID)
    {
        m_pGameManager->Change_Camera(static_cast<LEVEL>(m_pGameInstance->Get_LevelID()),
            TEXT("Chidori_Action_Camera"), nullptr);

        m_fCurrentHP -= 15.f;
        pNextState = CWhiteJetsu_ElectricShockState::Create(m_pNavigationCom, this);
        Set_Invincible(1.f);
    }

    if (m_fCurrentHP <= 0.f && false == m_isPlayingDeadAnim)
    {
        m_isPlayingDeadAnim = true;
        if (nullptr != pNextState)
        {
            pNextState->End();
            Safe_Release(pNextState);
        }
        pNextState = CWhiteJetsu_DeadState::Create(m_pNavigationCom, this);
    }

    Change_State(pNextState, false);*/

    m_fCurrentHP -= 1.f;
}

void CPajama::Change_State(CPajamaState* pNextState, _bool bBlend)
{
    _bool IsBlend = m_pState->End();
    Safe_Release(m_pState);

    pNextState->Start(bBlend);
    m_pState = pNextState;
}

HRESULT CPajama::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPajama::Initialize(void* pArg)
{
    CGameObject::GAMEOBJECT_DESC	Desc{};
    Desc.fRotationPerSec = XMConvertToRadians(180.0f);
    Desc.fSpeedPerSec = 10.f;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_HPBar()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_pGameManager->Add_TargetTransform(m_pTransformCom);

    /* 상태 초기화 및 시작. */
    m_pState = CPajama_IdleState::Create(m_pNavigationCom, this);
    m_pState->Start(true);

    return S_OK;
}

void CPajama::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CPajama::Update(_float fTimeDelta)
{
    Update_State(fTimeDelta);
    Update_SkillCoolDown(fTimeDelta);
    //m_pNavigationCom->Compute_Height(m_pTransformCom);

    __super::Update(fTimeDelta);

    m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

    _matrix PlayerMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
    _vector PlayerTranslation = m_pTransformCom->Get_State(STATE::POSITION);
    PlayerTranslation += m_pTransformCom->Get_State(STATE::LOOK) * 0.4f;
    PlayerMatrix.r[3] = PlayerTranslation;

    m_pHandAttackColliderCom->Update(PlayerMatrix);
}

void CPajama::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    /* 파자마의 몸통 콜라이더를 콜리전 매니저에 등록 */
    m_pGameManager->Add_Object_ToCollision(TEXT("Monster_Body"), this, m_pColliderCom);
    m_pGameManager->Add_Collider_ToCollision(TEXT("Monster_Attack"), COLLIDER_HANDLE_ID::ENEMY_JETSU_ATTACK, m_pHandAttackColliderCom);

    m_pNavigationCom->Compute_Height(m_pTransformCom);

    m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CPajama::Render()
{
#ifdef _DEBUG
    m_pColliderCom->Render();
    m_pHandAttackColliderCom->Render();
#endif

    return S_OK;
}

_bool CPajama::Use_Skill()
{
    if (m_fSkillTimeAcc >= m_fMaxSkillCoolDown)
    {
        m_fSkillTimeAcc = 0.f;
        return true;
    }
    else
        return false;
}

_bool CPajama::Use_Kunai()
{
    if (m_fKunaiTimeAcc >= m_fMaxKunaiCoolDown)
    {
        m_fKunaiTimeAcc = 0.f;
        return true;
    }
    else
        return false;
}

_bool CPajama::Use_Sliding()
{
    if (m_fSlidingTimeAcc >= m_fMaxSlidingCoolDown)
    {
        m_fSlidingTimeAcc = 0.f;
        return true;
    }
    else
        return false;
}

void CPajama::Update_SkillCoolDown(_float fTimeDelta)
{
    /* 쿨타임이 존재하는 여러 동작들에 대해 연산을 수행해줌. */
    m_fSkillTimeAcc += fTimeDelta;
    m_fKunaiTimeAcc += fTimeDelta;
    m_fSlidingTimeAcc += fTimeDelta;

    if (m_fSkillTimeAcc >= m_fMaxSkillCoolDown)
        m_fSkillTimeAcc = m_fMaxSkillCoolDown;

    if (m_fKunaiTimeAcc >= m_fMaxKunaiCoolDown)
        m_fKunaiTimeAcc = m_fMaxKunaiCoolDown;

    if (m_fSlidingTimeAcc >= m_fMaxSlidingCoolDown)
        m_fSlidingTimeAcc = m_fMaxSlidingCoolDown;
}

HRESULT CPajama::Ready_Components()
{
    /* Com_Navigation */
    CNavigation::NAVIGATION_DESC Desc;
    Desc.iCurrentCellIndex = 0;

    if (LEVEL::TUTORIAL == m_pGameManager->Get_NextLevel())
    {
        if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Navigation_Tutorial"),
            TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
            return E_FAIL;
    }
    else if (LEVEL::KONOHA_VILLAGE == m_pGameManager->Get_NextLevel())
    {
        if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Navigation_KonohaVillage"),
            TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &Desc)))
            return E_FAIL;
    }

    CBounding_Sphere::BOUNDING_SPHERE_DESC ColliderDesc{};

    ColliderDesc.fRadius = 0.7f;
    ColliderDesc.vCenter = _float3{ 0.f, 0.7f, 0.f };

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    ColliderDesc.isActive = false;

    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider_HandAttack"), reinterpret_cast<CComponent**>(&m_pHandAttackColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPajama::Ready_PartObjects()
{
    CUpper_Character::UPPER_PLAYER_DESC UpperDesc{};
    UpperDesc.pParentTransform = m_pTransformCom;
    UpperDesc.strModelName = TEXT("Prototype_Component_Model_Upper_Pajama");

    CHead_Character::HEAD_PLAYER_DESC HeadDesc{};
    HeadDesc.pParentTransform = m_pTransformCom;
    HeadDesc.strModelName = TEXT("Prototype_Component_Model_Head_Pajama");

    CFace_Character::tagFace_Player_Desc FaceDesc{};
    FaceDesc.pParentTransform = m_pTransformCom;
    FaceDesc.strModelName = TEXT("Prototype_Component_Model_Face_Pajama");

    /* Part_Upper */
    if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Upper_Player"),
        TEXT("Part_Upper"), &UpperDesc)))
        return E_FAIL;

    /* Part_Head */
    if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Head_Player"),
        TEXT("Part_Head"), &HeadDesc)))
        return E_FAIL;

    /* Part_Face */
    if (FAILED(__super::Add_PartObject(ENUM_CLASS(LEVEL::TUTORIAL), TEXT("Prototype_GameObject_Face_Player"),
        TEXT("Part_Face"), &FaceDesc)))
        return E_FAIL;

    return S_OK;
}

void CPajama::Update_State(_float fTimeDelta)
{
    CPajamaState* pNextState = { nullptr };
    pNextState = m_pState->Update(fTimeDelta);

    if (nullptr != pNextState)
    {
        _bool IsBlend = m_pState->End();
        //현재 스테이트 날려버림.
        Safe_Release(m_pState);

        //새로운 상태 시작해줌. (내부적으로 복서 들게 됨. 레퍼런스 카운트 증가 안함.)
        pNextState->Start(IsBlend);

        m_pState = pNextState;
    }
}

CPajama* CPajama::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
    CPajama* pInstance = new CPajama(pDevice, pContext, eObjectID);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CPajama");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPajama::Clone(void* pArg)
{
    CPajama* pInstance = new CPajama(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : CPajama");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPajama::Free()
{
    __super::Free();

    Safe_Release(m_pState);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pHandAttackColliderCom);
}
