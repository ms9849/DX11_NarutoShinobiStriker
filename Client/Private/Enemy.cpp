#include "Enemy.h"

#include "GameManager.h"

#include "Enemy_HPBar.h"
#include "Parts_Character.h"

CEnemy::CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CContainerObject{  pDevice, pContext, ENUM_CLASS(eObjectID) }
    , m_pGameManager { CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

CEnemy::CEnemy(const CEnemy& rhs)
    : CContainerObject { rhs }
    , m_pGameManager{ CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

_wstring CEnemy::Get_CurrentAnim()
{
    /* 애니메이션 이름 받아오기 */
    CParts_Character* pAnimParts = dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Upper")));
    if (nullptr != pAnimParts)
        return pAnimParts->Get_CurrentAnim();
}

void CEnemy::Set_Invincible(_float fInvincibleTime)
{
    m_fInvincibleTime = fInvincibleTime;
    m_IsInvincible = true;
}

_float CEnemy::Get_AnimProgress()
{
    /* 애니메이션 Progress 받아오기 */
    CParts_Character* pAnimParts = dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Upper")));
    if (nullptr != pAnimParts)
        return pAnimParts->Get_AnimProgress();
}

void CEnemy::Set_AnimProgress(_float fProgress)
{
    for (auto& iter : m_PartObjects)
    {
        static_cast<CParts_Character*>(iter.second)->Set_AnimProgress(fProgress);
    }
}

void CEnemy::Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate, _bool IsBlend, _float fBlendRatio, _bool IsLoop)
{
    /*
    애니메이션 바꿔주기. 상하체만 바뀐다.
    추후 한벌옷도 추가 예정.
    */
    for (auto& iter : m_PartObjects)
    {
        static_cast<CParts_Character*>(iter.second)->Set_AnimIndex(pAnimName, fAnimationPlayRate, IsBlend, fBlendRatio, IsLoop);
    }
}

_bool CEnemy::Play_Animation(_float fTimeDelta)
{
    _bool isAnimFinished = { false };

    isAnimFinished = dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Upper")))->Play_Animation(fTimeDelta);

    dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Face")))->Play_Animation(fTimeDelta);
    dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Head")))->Play_Animation(fTimeDelta);

    if(nullptr != Find_PartObject(TEXT("Part_Lower")))
        dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Lower")))->Play_Animation(fTimeDelta);

    if (nullptr != Find_PartObject(TEXT("Part_Weapon_R")))
        dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Weapon_R")))->Play_Animation(fTimeDelta);

    if (nullptr != Find_PartObject(TEXT("Part_Weapon_L")))
        dynamic_cast<CParts_Character*>(Find_PartObject(TEXT("Part_Weapon_L")))->Play_Animation(fTimeDelta);

    return isAnimFinished;
}

HRESULT CEnemy::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEnemy::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CEnemy::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CEnemy::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CEnemy::Late_Update(_float fTimeDelta)
{
    if (nullptr != m_pHPBar)
    {
        m_pHPBar->Set_HP(m_fCurrentHP, m_fMaxHP);
        m_pHPBar->Late_Update(fTimeDelta);
    }


    m_fInvincibleTime -= fTimeDelta;

    if (m_fInvincibleTime < 0.f)
    {
        m_fInvincibleTime = 0.f;
        m_IsInvincible = false;
    }

    __super::Late_Update(fTimeDelta);
}

HRESULT CEnemy::Render()
{
    return S_OK;
}

/* 추후 순수 가상함수로 작성할 것. */
void CEnemy::OnCollision(COLLIDER_HANDLE_ID eHandleID)
{
}

HRESULT CEnemy::Ready_HPBar()
{
    m_pHPBar = CEnemy_HPBar::Create(m_pDevice, m_pContext, OBJECTID::ENEMY_HPBAR);

    CEnemy_HPBar::ENEMY_HPBAR_DESC Desc;
    Desc.pTargetTransform = m_pTransformCom;

    m_pHPBar->Initialize(&Desc);

    return S_OK;
}

void CEnemy::Free()
{
    __super::Free();

    Safe_Release(m_pHPBar);
    Safe_Release(m_pGameManager);
}
