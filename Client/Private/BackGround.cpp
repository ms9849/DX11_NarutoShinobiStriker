#include "BackGround.h"

#include "GameInstance.h"

CBackGround::CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CUIObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CBackGround::CBackGround(const CBackGround& Prototype) 
	: CUIObject{ Prototype }
{
}

HRESULT CBackGround::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC Desc;
	//텍스쳐의 중심 x,y 위치 세팅
	Desc.fX = g_iWinSizeX / 2.f;
	Desc.fY = g_iWinSizeY / 2.f;
	//텍스쳐의 크기 세팅
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	return S_OK;
}

void CBackGround::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CBackGround::Update(_float fTimeDelta)
{
}

void CBackGround::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::PRIORITY, this);
}

HRESULT CBackGround::Render()
{	

	return S_OK;
}

HRESULT CBackGround::Ready_Components()
{

	///* Com_VIBuffer */
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
	//	TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	//	return E_FAIL;

	///* Com_Texture */
	//if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_BackGround"),
	//	TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//	return E_FAIL;

	return S_OK;
}

CBackGround* CBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CBackGround* pInstance = new CBackGround(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBackGround::Clone(void* pArg)
{
	CBackGround* pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGround::Free()
{
	__super::Free();

	
}
