#include "Transform.h"
#include "Shader.h"

#include "GameInstance.h"
CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{

}

CTransform::CTransform(const CTransform& Prototype)
	: CComponent { Prototype }
{

}

_float3 CTransform::Get_Scale() const
{	
	return _float3(
		XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))),
		XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
		XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK)))
	);
}

void CTransform::Set_Scale(_float fX, _float fY, _float fZ)
{
	Set_State(STATE::RIGHT, XMVector3Normalize(Get_State(STATE::RIGHT)) * fX);
	Set_State(STATE::UP, XMVector3Normalize(Get_State(STATE::UP))* fY);
	Set_State(STATE::LOOK, XMVector3Normalize(Get_State(STATE::LOOK))* fZ);
}

void CTransform::Set_Position(_fvector vPosition, CNavigation* pNavigation)
{
	_vector		vOriginPos = Get_State(STATE::POSITION);

	_vector		vDiff =  vPosition - vOriginPos;

	_float3 vSliding = {};

	if ((nullptr == pNavigation) ||
		(true == pNavigation->isMove(vPosition, &vSliding)))
		Set_State(STATE::POSITION, vPosition);

	else if (true == pNavigation->isMove(vOriginPos + vDiff, nullptr))
		Set_State(STATE::POSITION, vOriginPos + 0.2f * m_fSpeedPerSec * XMVector3Normalize(XMLoadFloat3(&vSliding)));
}

HRESULT CTransform::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	/* 월드 행렬 Identity 수행, Store함수로 저장해주는 것을 볼 수 있다 */
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	if (nullptr == pArg)
		return S_OK;

	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRotationPerSec = pDesc->fRotationPerSec;

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

void CTransform::Go_Straight(_float fTimeDelta, class CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vLook = Get_State(STATE::LOOK);	

	/* DX9 때와는 다르게 기존 벡터 원형을 수정하지 않는다. SIMD라고 해도 값으로 넘기는건데 빠른가..? */
	_vector		vDirection = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
	vPosition += vDirection;

	_float3 vSliding = {};

	if ((nullptr == pNavigation) ||
		(true == pNavigation->isMove(vPosition, &vSliding)))
		Set_State(STATE::POSITION, vPosition);

	else if (true == pNavigation->isMove(Get_State(STATE::POSITION) + fTimeDelta * m_fSpeedPerSec * XMVector3Normalize((XMLoadFloat3(&vSliding))), nullptr))
		Set_State(STATE::POSITION, Get_State(STATE::POSITION) + fTimeDelta * m_fSpeedPerSec * XMVector3Normalize(XMLoadFloat3(&vSliding)));
}

void CTransform::Go_Backward(_float fTimeDelta, class CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vLook = Get_State(STATE::LOOK);

	_vector		vDirection = -1.f * XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	vPosition += vDirection;

	_float3 vSliding = {};

	if ((nullptr == pNavigation) ||
		(true == pNavigation->isMove(vPosition, &vSliding)))
		Set_State(STATE::POSITION, vPosition);

	else if (true == pNavigation->isMove(Get_State(STATE::POSITION) + fTimeDelta * m_fSpeedPerSec * XMVector3Normalize(XMLoadFloat3(&vSliding)), nullptr))
		Set_State(STATE::POSITION, Get_State(STATE::POSITION) + fTimeDelta * m_fSpeedPerSec * XMVector3Normalize(XMLoadFloat3(&vSliding)));
}

void CTransform::Go_Left(_float fTimeDelta, class CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vRight = Get_State(STATE::RIGHT);

	_vector		vDirection = -1.f * XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;
	vPosition += vDirection;

	_float3 vSliding = {};

	if ((nullptr == pNavigation) ||
		(true == pNavigation->isMove(vPosition, &vSliding)))
		Set_State(STATE::POSITION, vPosition);

	else if (true == pNavigation->isMove(Get_State(STATE::POSITION) + fTimeDelta * m_fSpeedPerSec * (XMLoadFloat3(&vSliding)), nullptr))
		Set_State(STATE::POSITION, Get_State(STATE::POSITION) +  fTimeDelta * m_fSpeedPerSec * (XMLoadFloat3(&vSliding)));
}

void CTransform::Go_Right(_float fTimeDelta, class CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vRight = Get_State(STATE::RIGHT);

	_vector		vDirection = XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;
	vPosition += vDirection;

	_float3 vSliding = {};

	if ((nullptr == pNavigation) ||
		(true == pNavigation->isMove(vPosition, &vSliding)))
		Set_State(STATE::POSITION, vPosition);

	else if (true == pNavigation->isMove(Get_State(STATE::POSITION) + fTimeDelta * m_fSpeedPerSec * XMVector3Normalize(XMLoadFloat3(&vSliding)), nullptr))
		Set_State(STATE::POSITION, Get_State(STATE::POSITION) + fTimeDelta * m_fSpeedPerSec * XMVector3Normalize(XMLoadFloat3(&vSliding)));
}

void CTransform::Go_Direction(_fvector vDir, _float fTimeDelta, class CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE::POSITION);
	
	/* 방향은 정규화해야지 */
	_vector vDirection = XMVectorSetW(XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta, 0.f);
	vPosition += vDirection; 

	_float3 vSliding = {};

	if ((nullptr == pNavigation) ||
		(true == pNavigation->isMove(vPosition, &vSliding)))
		Set_State(STATE::POSITION, vPosition);

	else if (true == pNavigation->isMove(Get_State(STATE::POSITION) + fTimeDelta * m_fSpeedPerSec * XMVector3Normalize(XMLoadFloat3(&vSliding)), nullptr))
		Set_State(STATE::POSITION, Get_State(STATE::POSITION) +  fTimeDelta * m_fSpeedPerSec * XMVector3Normalize(XMLoadFloat3(&vSliding)));
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE::RIGHT);
	_vector		vUp = Get_State(STATE::UP);
	_vector		vLook = Get_State(STATE::LOOK);	

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3		vScale = Get_Scale();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);
}

void CTransform::Rotation(_float fRadianX, _float fRadianY, _float fRadianZ)
{
	_float3		vScale = Get_Scale();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	/* 짐벌락 : 복합적인 회전을 적용하다보면 특정 축 회전이 이전 축 회전을 상쇄시켜버리는 경우가 발생하여 */
	/* 특정 축에 대한 회전이 무효화되는 상황이 발생한다. */
	/*_matrix		RotationMatrix = XMMatrixRotationX(fRadianX) * XMMatrixRotationY(fRadianY) * XMMatrixRotationZ(fRadianZ);*/

	/* 복소수가 회전을 의미한다. (a + bi)*/
	/* w + xi + yj + zk */
	_vector		vQuternion = XMQuaternionRotationRollPitchYaw(fRadianX, fRadianY, fRadianZ);
	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vQuternion);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);
}

void CTransform::Orbit(_fvector vAxisPos, _fvector vAxis, _float fRadian)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	_float4x4	matDest = {};
	XMStoreFloat4x4(&matDest, RotationMatrix);

	/* Rotation에 Pos 적용 */
	XMStoreFloat4(reinterpret_cast<_float4*>(&matDest.m[3]), vAxisPos);
	RotationMatrix = XMLoadFloat4x4(&matDest);

	/* 행렬끼리 곱한 뒤 세팅.*/
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&m_WorldMatrix), RotationMatrix));
}

/* up이 늘 0 1 0으로 고정되게 해주는 상황. 즉 right 축으로의 회전은 강제로 없앤다. */
void CTransform::Change_Look_Force(_fvector vLook)
{
	_float3		vScale = Get_Scale();
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVector3Normalize(vLook));

	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE::LOOK, XMVector3Normalize(XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f)) * vScale.z));
}

void CTransform::LookAt(_fvector vAt)
{
	_vector     vPosition = Get_State(STATE::POSITION);
	_float3		vScale = Get_Scale();
	_vector		vRight, vUp, vLook;

	vLook = vAt - vPosition;

	vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE::UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookAt_XZ(_fvector vAt)
{
	_vector     vPosition = Get_State(STATE::POSITION);
	_float3		vScale = Get_Scale();
	_vector		vRight, vUp, vLook;

	vLook = vAt - vPosition;
	vLook = XMVectorSetW(XMVectorSetY(vLook, 0.f), 0.f);

	vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE::UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookAt_Lerp(_fvector vAt, _float fRatio)
{
	_float3		vScale = Get_Scale();

	_vector		vRight, vUp, vLook;
	_vector		vTarget = XMVector3Normalize(vAt - Get_State(STATE::POSITION));
	vTarget = XMVectorSetW(vTarget, 0.f);

	vLook = XMVector3Normalize(XMVectorLerp(Get_State(STATE::LOOK), vTarget, fRatio));
	vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE::UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::Chase(_fvector vTargetPos, _float fTimeDelta, class CNavigation* pNavigation, _float fLimitDistance)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vDirection = vTargetPos - vPosition;
	_float		fDist = XMVectorGetX(XMVector3Length(vTargetPos - vPosition));

	if (fDist < fLimitDistance)
		return;

	_vector		vMove;

	vMove = XMVector3Normalize(vDirection) * m_fSpeedPerSec * fTimeDelta;


	_float3 vSliding = {};

	vPosition += vMove;
	if (nullptr == pNavigation ||
		true == pNavigation->isMove(vPosition, &vSliding))
		Set_State(STATE::POSITION, vPosition);

	else if (true == pNavigation->isMove(Get_State(STATE::POSITION) +  fTimeDelta * m_fSpeedPerSec * XMVector3Normalize(XMLoadFloat3(&vSliding)), nullptr))
		Set_State(STATE::POSITION, Get_State(STATE::POSITION) +  fTimeDelta * m_fSpeedPerSec * XMVector3Normalize(XMLoadFloat3(&vSliding)));
}

void CTransform::Chase_XZ(_fvector vTargetPos, _float fTimeDelta, CNavigation* pNavigation, _float fLimitDistance)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vDirection = vTargetPos - vPosition;

	vDirection = XMVectorSetY(vDirection, 0.f);

	_float		fDist = XMVectorGetX(XMVector3Length(vTargetPos - vPosition));

	if (fDist < fLimitDistance)
		return;

	_vector		vMove;

	_float3 vSliding = {};
	
	vMove = XMVector3Normalize(vDirection) * m_fSpeedPerSec * fTimeDelta;
	vMove = XMVectorSetY(vMove, 0.f);

 	vPosition += vMove;
	if (nullptr == pNavigation ||
		true == pNavigation->isMove(vPosition, &vSliding))
		Set_State(STATE::POSITION, vPosition);

	/* 멀어지는데도 계속 쫓아옴 */
	/* 방법 1. 플레이어가 네비메시를 벗어나지 않던가 */
	/* 방법 2. 슬라이딩 이후의 거리를 한번 더 비교해서 이동할지 말지 선택 */
	else if(true == pNavigation->isMove(Get_State(STATE::POSITION) + fTimeDelta * m_fSpeedPerSec * XMVector3Normalize(XMLoadFloat3(&vSliding)), nullptr))
		Set_State(STATE::POSITION, Get_State(STATE::POSITION) + fTimeDelta * m_fSpeedPerSec * XMVector3Normalize(XMLoadFloat3(&vSliding)));
}

void CTransform::Chase_Lerp(_fvector vTargetPos, _float fTimeDelta, _float fLimitDistance)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vDirection = vTargetPos - vPosition;
	_float		fDist = XMVectorGetX(XMVector3Length(vTargetPos - vPosition));

	_vector		vLerp = XMVectorSetW(XMVectorLerp(vPosition, vTargetPos, fTimeDelta * m_fSpeedPerSec), 1.f);

	if (fDist < fLimitDistance)
		return;

	Set_State(STATE::POSITION, vLerp);
}

void CTransform::Mul_Parent(_fmatrix pParentMatrixPtr)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&m_WorldMatrix), pParentMatrixPtr));
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();

}
