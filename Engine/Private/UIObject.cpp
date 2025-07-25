#include "UIObject.h"

#include "GameInstance.h"
#include "Orthogonal.h"

CUIObject::CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CUIObject::CUIObject(const CUIObject& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CUIObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUIObject::Initialize(void* pArg)
{
    UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);
    //fx, fy, fsizeX, fsizeY 다 있으니까 세팅 해주던가 하고..

    if (FAILED(m_pOrthogonalCom->Initialize()))
        return E_FAIL;

    return S_OK;
}

void CUIObject::Priority_Update(_float fTimeDelta)
{
}

void CUIObject::Update(_float fTimeDelta)
{
}

void CUIObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CUIObject::Render()
{
    return S_OK;
}

void CUIObject::Free()
{
    __super::Free();

}
