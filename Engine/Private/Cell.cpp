#include "Cell.h"

CCell::CCell()
{
}

/* 셀들의 좌표는 월드 기준으로 잡히는 걸 명심할 것. */

HRESULT CCell::Initialize(const _float3* vPoints, _uint iIndex)
{
	memcpy(m_vPoints, vPoints, sizeof(_float3) * ENUM_CLASS(NAVI_POINT::END));

	_float3		vLines[ENUM_CLASS(NAVI_LINE::END)] = {};

	/* 포인트 A B C를 이용하여 선분 AB, BC, CA 저장 */
	XMStoreFloat3(&vLines[ENUM_CLASS(NAVI_LINE::AB)],
		XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::B)]) - XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::A)]));
	XMStoreFloat3(&vLines[ENUM_CLASS(NAVI_LINE::BC)],
		XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::C)]) - XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::B)]));
	XMStoreFloat3(&vLines[ENUM_CLASS(NAVI_LINE::CA)],
		XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::A)]) - XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::C)]));

	/* 포인트 A B C*/
	for (_uint i = 0; i < ENUM_CLASS(NAVI_LINE::END); ++i)
		m_vNormals[i] = _float3(vLines[i].z * -1.f, 0.f, vLines[i].x);

	m_iIndex = iIndex;

	return S_OK;
}

CCell* CCell::Create(const _float3* vPoints, _uint iIndex)
{
	CCell* pInstance = new CCell();

	if (FAILED(pInstance->Initialize(vPoints, iIndex)))
	{
		MSG_BOX("Creeate Failed ! : CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
	__super::Free();
}
