#include "Navigation.h"

#include "Cell.h"

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation& Prototype)
	: CComponent { Prototype }
	, m_Cells{ Prototype.m_Cells }
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);
}

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationDataFiles)
{
	_ulong		dwByte = { };
	HANDLE		hFile = CreateFile(pNavigationDataFiles, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		_float3		vPoints[ENUM_CLASS(NAVI_POINT::END)] = {};

		ReadFile(hFile, vPoints, sizeof(_float3) * ENUM_CLASS(NAVI_POINT::END), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		CCell* pCell = CCell::Create(vPoints, m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);
	
	/* 현재 타고 있는 셀의 인덱스를 구해낸다. */
	m_iCurrentCellIndex = pDesc->iCurrentCellIndex;

	return S_OK;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFiles)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationDataFiles)))
	{
		MSG_BOX("Create Failed : Navigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create Failed : Navigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();
}
