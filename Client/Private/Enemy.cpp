#include "Enemy.h"

#include "GameManager.h"

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
}

HRESULT CEnemy::Render()
{
    return S_OK;
}

void CEnemy::Free()
{
    __super::Free();
}
