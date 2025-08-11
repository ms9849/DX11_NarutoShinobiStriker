#include "Panel.h"

#include "TestButton.h"
#include "GameInstance.h"
#include "GameManager.h"

CPanel::CPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CUIObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
    , m_pGameManager { CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

CPanel::CPanel(const CPanel& rhs)
    : CUIObject { rhs }
    , m_pGameManager{ CGameManager::GetInstance() }
{
    Safe_AddRef(m_pGameManager);
}

HRESULT CPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPanel::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CPanel::Priority_Update(_float fTimeDelta)
{
}

void CPanel::Update(_float fTimeDelta)
{
}

void CPanel::Late_Update(_float fTimeDelta)
{

}

HRESULT CPanel::Render()
{
    __super::Render();

    return S_OK;
}

HRESULT CPanel::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

void CPanel::Free()
{
    __super::Free();

    for (auto& iter : m_Childs)
        Safe_Release(iter);

    m_Childs.clear();

    Safe_Release(m_pGameManager);
}
