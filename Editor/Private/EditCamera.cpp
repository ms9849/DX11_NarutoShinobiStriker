#include "EditCamera.h"

#include "GameInstance.h"

CEditCamera::CEditCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID)
	: CCamera{ pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CEditCamera::CEditCamera(const CEditCamera& rhs)
	: CCamera { rhs }
{
}

void CEditCamera::Set_Desc(void* pArg)
{
	EDIT_CAMERA_DESC* pDesc = static_cast<EDIT_CAMERA_DESC*>(pArg);

	m_fMouseSensivity = pDesc->fMouseSensitiy;
	m_fSpeedFactor = pDesc->fSpeedPerSec;
}

void CEditCamera::Key_Input(_float fTimeDelta)
{
	m_pTransformCom->Go_Straight(fTimeDelta * (m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::W)) * 0.2f);

	if (false == m_IsCameraActivated || m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::RBUTTON))
		return;

	if (m_pGameInstance->Key_Pressing(DIK_W))
		m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeedFactor);

	if (m_pGameInstance->Key_Pressing(DIK_S))
		m_pTransformCom->Go_Backward(fTimeDelta * m_fSpeedFactor);

	if (m_pGameInstance->Key_Pressing(DIK_A))
		m_pTransformCom->Go_Left(fTimeDelta * m_fSpeedFactor);

	if (m_pGameInstance->Key_Pressing(DIK_D))
		m_pTransformCom->Go_Right(fTimeDelta * m_fSpeedFactor);

	if (m_pGameInstance->Key_Pressing(DIK_F7))
	{
		_float4 vTargetPos = { 30.f, 30.f, 30.f, 1.f };
		m_pTransformCom->Chase_Lerp(XMLoadFloat4(&vTargetPos), fTimeDelta, 2.f);
	}

	_long iMouseMove = { 0 };

	if (iMouseMove = m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * iMouseMove * m_fMouseSensivity);
	}

	if (iMouseMove = m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * iMouseMove * m_fMouseSensivity);
	}

}

HRESULT CEditCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEditCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fMouseSensivity = static_cast<EDIT_CAMERA_DESC*>(pArg)->fMouseSensitiy;

	return S_OK;
}

void CEditCamera::Priority_Update(_float fTimeDelta)
{
	Key_Input(fTimeDelta);

	__super::Bind_Matrices();
}

void CEditCamera::Update(_float fTimeDelta)
{
}

void CEditCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CEditCamera::Render()
{
	return S_OK;
}

CEditCamera* CEditCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID)
{
	CEditCamera* pInstance = new CEditCamera(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CREATE FAILED : Edit Camera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEditCamera::Clone(void* pArg)
{
	CEditCamera* pInstance = new CEditCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CLONE FAILED : Edit Camera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEditCamera::Free()
{
	__super::Free();
}
