#include "Pooling.h"

#include "GameObject.h"
#include "GameInstance.h"

CPooling::CPooling(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) 
	: CComponent { pDevice, pContext }
{
}

CPooling::CPooling(const CPooling& Prototype)
	: CComponent { Prototype }
{
}

HRESULT CPooling::Initialize_Prototype()
{
	/* 여기선 아무것도 하지 않는다. */
	return S_OK;
}

HRESULT CPooling::Initialize(void* pArg)
{
	/* 
	Desc를 통해 풀링할 객체의 갯수, 문자열 태그, 객체 원본..을 받아올까? 
	객체 원본은 this로 받아온다 쳐도..
	*/

	POOLING_DESC* pDesc = reinterpret_cast<POOLING_DESC*>(pArg);
	m_strPoolingTag = pDesc->strPoolingTag;

	/* 클라단에서 풀링 매니저에 넘겨줘서 세팅할 수 있게 해줌 */
	return S_OK;
}

CPooling* CPooling::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPooling* pInstance = new CPooling(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : Pooling Component");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CPooling::Clone(void* pArg)
{
	CPooling* pInstance = new CPooling(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : Pooling Component");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPooling::Free()
{
	__super::Free();
}
