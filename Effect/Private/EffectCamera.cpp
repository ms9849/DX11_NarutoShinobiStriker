#include "EffectCamera.h"

#include "GameInstance.h"

CEffectCamera::CEffectCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID)
	: CCamera{ pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CEffectCamera::CEffectCamera(const CEffectCamera& rhs)
	: CCamera { rhs }
{
}

void CEffectCamera::Set_Desc(void* pArg)
{
	EFFECT_CAMERA_DESC* pDesc = static_cast<EFFECT_CAMERA_DESC*>(pArg);

	m_fMouseSensivity = pDesc->fMouseSensitiy;
	m_fSpeedFactor = pDesc->fSpeedPerSec;
}

void CEffectCamera::Key_Input(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_F2))
		m_IsCameraActivated = !m_IsCameraActivated;

	if (false == m_IsCameraActivated || m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::RBUTTON))
		return;

	m_pTransformCom->Go_Straight(fTimeDelta * (m_pGameInstance->Get_MouseMove(MOUSEMOVESTATE::W)) * 0.2f);

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

HRESULT CEffectCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffectCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fMouseSensivity = static_cast<EFFECT_CAMERA_DESC*>(pArg)->fMouseSensitiy;

	return S_OK;
}

void CEffectCamera::Priority_Update(_float fTimeDelta)
{
	Key_Input(fTimeDelta);

	__super::Bind_Matrices();
}

void CEffectCamera::Update(_float fTimeDelta)
{
}

void CEffectCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CEffectCamera::Render()
{
	return S_OK;
}

CEffectCamera* CEffectCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID)
{
	CEffectCamera* pInstance = new CEffectCamera(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CREATE FAILED : Edit Camera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffectCamera::Clone(void* pArg)
{
	CEffectCamera* pInstance = new CEffectCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CLONE FAILED : Edit Camera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectCamera::Free()
{
	__super::Free();
}
