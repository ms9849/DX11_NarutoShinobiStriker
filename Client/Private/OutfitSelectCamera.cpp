#include "OutfitSelectCamera.h"

#include "GameInstance.h"
COutfitSelectCamera::COutfitSelectCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CCamera { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

COutfitSelectCamera::COutfitSelectCamera(const COutfitSelectCamera& rhs)
	: CCamera { rhs }
{
}

void COutfitSelectCamera::Key_Input()
{
	if (m_pGameInstance->Key_Down(DIK_1))
	{
		m_eSelectState = SELECT_STATE::HEAD;
	}

	else if (m_pGameInstance->Key_Down(DIK_2))
	{
		m_eSelectState = SELECT_STATE::UPPER;
	}

	else if (m_pGameInstance->Key_Down(DIK_3))
	{
		m_eSelectState = SELECT_STATE::LOWER;
	}

	else if (m_pGameInstance->Key_Down(DIK_4))
	{
		m_eSelectState = SELECT_STATE::BODY;
	}
}

HRESULT COutfitSelectCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT COutfitSelectCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void COutfitSelectCamera::Priority_Update(_float fTimeDelta)
{
	__super::Bind_Matrices();
}

void COutfitSelectCamera::Update(_float fTimeDelta)
{
	Key_Input();
	Camera_Action(fTimeDelta);
}

void COutfitSelectCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT COutfitSelectCamera::Render()
{
	return S_OK;
}

void COutfitSelectCamera::Camera_Action(_float fTimeDelta)
{
	/* 타입에 따라 카메라의 위치를 변경할 것. */
	if (SELECT_STATE::HEAD == m_eSelectState)
	{
		m_pTransformCom->LookAt_Lerp(XMVectorSet(0.f, 1.f, -1.f, 1.f));
		m_pTransformCom->Chase_Lerp(XMVectorSet(0.f, 0.5f, 0.5f, 1.f), fTimeDelta, 0.2f);
	}

	else if (SELECT_STATE::UPPER == m_eSelectState)
	{
		m_pTransformCom->LookAt_Lerp(XMVectorSet(0.f, 0.7f, -0.7f, 1.f));
		m_pTransformCom->Chase_Lerp(XMVectorSet(0.f, 1.f, 1.f, 1.f), fTimeDelta, 0.2f);
	}

	else if (SELECT_STATE::LOWER == m_eSelectState)
	{
		m_pTransformCom->LookAt_Lerp(XMVectorSet(0.f, 0.4f, -0.4f, 1.f));
		m_pTransformCom->Chase_Lerp(XMVectorSet(0.f, 1.f, 0.5f, 1.f), fTimeDelta, 0.2f);
	}
	else if (SELECT_STATE::BODY == m_eSelectState)
	{
		m_pTransformCom->LookAt_Lerp(XMVectorSet(0.f, 1.f, -1.f, 1.f));
		m_pTransformCom->Chase_Lerp(XMVectorSet(0.f, 1.5f, 1.5f, 1.f), fTimeDelta, 0.2f);
	}
}

COutfitSelectCamera* COutfitSelectCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	COutfitSelectCamera* pInstance = new COutfitSelectCamera(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : OutfitSelectCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* COutfitSelectCamera::Clone(void* pArg)
{
	COutfitSelectCamera* pInstance = new COutfitSelectCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : OutfitSelectCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COutfitSelectCamera::Free()
{
	__super::Free();
}
