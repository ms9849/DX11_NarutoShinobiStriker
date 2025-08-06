#include "TestCamera.h"

#include "GameInstance.h"

CTestCamera::CTestCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CCamera { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CTestCamera::CTestCamera(const CTestCamera& rhs)
	: CCamera { rhs }
{
}

HRESULT CTestCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTestCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTestCamera::Priority_Update(_float fTimeDelta)
{
	m_pGameInstance->Set_CameraWorldMatrix(*m_pTransformCom->Get_WorldMatrixPtr());
}

void CTestCamera::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(VK_UP))
		m_pTransformCom->Go_Straight(fTimeDelta);

	if (m_pGameInstance->Key_Pressing(VK_DOWN))
		m_pTransformCom->Go_Backward(fTimeDelta);

	if (m_pGameInstance->Key_Pressing(VK_LEFT))
		m_pTransformCom->Go_Left(fTimeDelta);

	if (m_pGameInstance->Key_Pressing(VK_RIGHT))
		m_pTransformCom->Go_Right(fTimeDelta);
}

void CTestCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CTestCamera::Render()
{
	return S_OK;
}

CTestCamera* CTestCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CTestCamera* pInstance = new CTestCamera(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CREATE FAILED : TESTCAMERA");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTestCamera::Clone(void* pArg)
{
	CTestCamera* pInstance = new CTestCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CLONE FAILED : TESTCAMERA");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestCamera::Free()
{
	__super::Free();
}
