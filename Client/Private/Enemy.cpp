#include "Enemy.h"

#include "GameManager.h"

#include "Enemy_HPBar.h"


CEnemy::CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CGameObject{  pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CEnemy::CEnemy(const CEnemy& rhs)
    : CGameObject { rhs }
{
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
}

void CEnemy::Update(_float fTimeDelta)
{
}

void CEnemy::Late_Update(_float fTimeDelta)
{
    m_pHPBar->Set_HP(m_fCurrentHP, m_fMaxHP);
    m_pHPBar->Late_Update(fTimeDelta);
}

HRESULT CEnemy::Render()
{
    return S_OK;
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
}
