#include "UIObject.h"

#include "GameInstance.h"
#include "Orthogonal.h"

CUIObject::CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iObjectID)
    : CGameObject{ pDevice, pContext, iObjectID }
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
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    
    m_pOrthogonalCom = COrthogonal::Create(m_pDevice, m_pContext);
    if (m_pOrthogonalCom == nullptr)
        return E_FAIL;

    if (FAILED(m_pOrthogonalCom->Initialize()))
        return E_FAIL;


    D3D11_VIEWPORT ViewPortDesc{};
    //앞으로는 뷰포트가 여러개가 될거고, 갯수를 입력받는게 당연해질 것이라고 하심.
    _uint          iNumViewPorts = { 1 };

    m_pContext->RSGetViewports(&iNumViewPorts, &ViewPortDesc);

    UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);

    m_fX = pDesc->fX;
    m_fY = pDesc->fY;
    m_fZ = pDesc->fZ;

    m_pTransformCom->Set_Scale(pDesc->fSizeX, pDesc->fSizeY, 1.f);

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - ViewPortDesc.Width / 2.f, -1.f * m_fY + ViewPortDesc.Height / 2.f, m_fZ, 1.f));

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

    Safe_Release(m_pOrthogonalCom);
}
