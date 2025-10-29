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

void CCamera::Shake(_float fShakeTime, _float fIntensity)
{
	m_IsShake = true;

	m_fIntensity = fIntensity;
	m_fShakeTime = fShakeTime;
	m_fShakeTimeAcc = 0.f;
	XMStoreFloat4(&m_vOriginPos, m_pTransformCom->Get_State(STATE::POSITION));
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

	/*EYE랑 At으로 카메라 트랜스폼도 세팅해줘야 한다*/
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&pDesc->vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&pDesc->vAt));

	_uint iNumViewPorts = { 1 };
	D3D11_VIEWPORT ViewPort{};

	m_pContext->RSGetViewports(&iNumViewPorts, &ViewPort);

	m_fAspect = ViewPort.Width / ViewPort.Height;
	m_fFovy = pDesc->fFovy;
	m_fFar = pDesc->fFar;
	m_fNear = pDesc->fNear;

	return S_OK;
}

void CCamera::Priority_Update(_float fTimeDelta)
{
}

void CCamera::Update(_float fTimeDelta)
{

	if (true == m_IsShake)
	{
		m_fShakeTimeAcc += fTimeDelta;
		_vector vRight = m_pTransformCom->Get_State(STATE::RIGHT);
		_vector vUp = m_pTransformCom->Get_State(STATE::UP);

		// 정규화 (혹시 모르니)
		vRight = XMVector3Normalize(vRight);
		vUp = XMVector3Normalize(vUp);

		// 감쇠 (시간이 지날수록 약하게)
		_float fAttenuation = 1.f - (m_fShakeTimeAcc / m_fShakeTime);
		fAttenuation = max(0.f, fAttenuation);

		// 좌우, 상하 랜덤 흔들림 생성
		_float fOffsetRight = m_pGameInstance->Random_Normal() * m_fIntensity * fAttenuation;
		_float fOffsetUp = m_pGameInstance->Random_Normal() * m_fIntensity * fAttenuation;

		// 오프셋 벡터 계산 (Right + Up 방향으로만 흔들림)
		_vector vShakeOffset = vRight * fOffsetRight + vUp * fOffsetUp;

		// 원래 위치 기준으로 오프셋 적용
		_vector vNewPos = XMLoadFloat4(&m_vOriginPos) + vShakeOffset;

		// 카메라 위치 갱신
		m_pTransformCom->Set_State(STATE::POSITION, vNewPos);

		if (m_fShakeTime <= m_fShakeTimeAcc)
		{
			m_IsShake = false;
			m_fShakeTimeAcc = 0.f;

			m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_vOriginPos));
		}
	}
}

void CCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

HRESULT CCamera::Bind_Matrices()
{
	m_pGameInstance->Set_Pipeline_Matrix(D3DTS::VIEW, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr())));
	m_pGameInstance->Set_Pipeline_Matrix(D3DTS::PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));

	return S_OK;
}

void CCamera::Free()
{
	__super::Free();
}
