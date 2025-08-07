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

	m_fMouseSensivity = static_cast<TEST_CAMERA_DESC*>(pArg)->fMouseSensitiy;

	return S_OK;
}

void CTestCamera::Priority_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_W))
		m_pTransformCom->Go_Straight(fTimeDelta);

	if (m_pGameInstance->Key_Pressing(DIK_S))
		m_pTransformCom->Go_Backward(fTimeDelta);

	if (m_pGameInstance->Key_Pressing(DIK_A))
		m_pTransformCom->Go_Left(fTimeDelta);

	if (m_pGameInstance->Key_Pressing(DIK_D))
		m_pTransformCom->Go_Right(fTimeDelta);

	_long iMouseMove = {0};

	if (iMouseMove = m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * iMouseMove * m_fMouseSensivity);
	}

	if (iMouseMove = m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * iMouseMove * m_fMouseSensivity);
	}

	__super::Bind_Matrices();
}

void CTestCamera::Update(_float fTimeDelta)
{
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
