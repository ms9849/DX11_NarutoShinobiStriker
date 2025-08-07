#include "Panel.h"

#include "TestButton.h"
#include "GameInstance.h"

CPanel::CPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CUIObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CPanel::CPanel(const CPanel& rhs)
    : CUIObject { rhs }
{
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

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);

    for (auto& iter : m_Childs)
        Safe_Release(iter);

    m_Childs.clear();
}
