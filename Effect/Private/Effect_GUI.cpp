#include "Effect_GUI.h"

CEffect_GUI::CEffect_GUI()
{
}

HRESULT CEffect_GUI::Initialize()
{
	return S_OK;
}

CEffect_GUI* CEffect_GUI::Create()
{
	CEffect_GUI* pInstance = new CEffect_GUI();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Create Failed : Effect Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_GUI::Free()
{
	__super::Free();
}
