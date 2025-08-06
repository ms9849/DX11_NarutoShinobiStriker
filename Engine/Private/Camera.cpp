#include "Camera.h"

#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint eObjectID)
    : CGameObject { pDevice, pContext, eObjectID }
{
}

CCamera::CCamera(const CCamera& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr == pArg)
		return E_FAIL;

	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);

	_uint iNumViewPorts = { 1 };
	D3D11_VIEWPORT ViewPort{};

	m_pContext->RSGetViewports(&iNumViewPorts, &ViewPort);

	m_fAspect = ViewPort.Width / ViewPort.Height;
	m_fFovy = pDesc->fFovy;
	m_fFar = pDesc->fFar;
	m_fNear = pDesc->fNear;

	/*EYE랑 At으로 카메라 트랜스폼도 세팅해줘야 한다*/

	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&pDesc->vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&pDesc->vAt));

	_float4x4 ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMLoadFloat4(&pDesc->vEye), XMLoadFloat4(&pDesc->vAt), XMVectorSet(0.f, 1.f, 0.f, 1.f)));
	m_pGameInstance->Set_CameraWorldMatrix(*m_pTransformCom->Get_WorldMatrixPtr());

	_float4x4 ProjMatrix;
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
	m_pGameInstance->Set_ProjMatrix(ProjMatrix);

	return S_OK;
}

void CCamera::Priority_Update(_float fTimeDelta)
{
}

void CCamera::Update(_float fTimeDelta)
{
}

void CCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
    return S_OK;
}

void CCamera::Free()
{
	__super::Free();
}
