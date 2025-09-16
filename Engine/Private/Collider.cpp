#include "Collider.h"

#include "Bounding_AABB.h"
#include "GameInstance.h"


CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CCollider::CCollider(const CCollider& Prototype)
	: CComponent{ Prototype }
	, m_eType{ Prototype.m_eType }
#ifdef _DEBUG
	, m_pBatch{ Prototype.m_pBatch }
	, m_pEffect{ Prototype.m_pEffect }
	, m_pInputLayout{ Prototype.m_pInputLayout }
#endif

{

#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif

}

HRESULT CCollider::Initialize_Prototype(COLLIDER eType)
{
	m_eType = eType;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t		iShaderByteCodeLenght = {};

	/* 미리 내장된 셰이더를 꺼내옴 */
	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLenght);

	/* 
	Primitive Batch, Basic Effect에 맞는 
	레이아웃을 미리 생성하여 들고 있게 한다.
	패스는 하나뿐이기 떄문에, 레이아웃도 하나만 들고 있으면 된다.
	*/
	if (m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLenght, &m_pInputLayout) < 0)
		return E_FAIL;
#endif


	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
  	switch (m_eType)
	{
	case COLLIDER::AABB:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pArg);
		break;
	case COLLIDER::OBB:
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, pArg);
		break;
	case COLLIDER::SPHERE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, pArg);
		break;
	}

	return S_OK;
}

void CCollider::Update(_fmatrix WorldMatrix)
{
	m_isColl = false;
	m_pBounding->Update(WorldMatrix);
}

HRESULT CCollider::Render()
{
	if (nullptr == m_pBounding)
		return E_FAIL;

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_PipeLine_Matrix(D3DTS::VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_PipeLine_Matrix(D3DTS::PROJ));

	m_pContext->IASetInputLayout(m_pInputLayout);
	m_pEffect->Apply(m_pContext);

	/* 렌더 결과 리턴. */
	/* 
	만약 충돌하지 않았다면 초록색,
	충돌했다면 붉은색으로 표시한다.
	*/
	return m_pBounding->Render(m_pBatch, 
		false == m_pBounding->Get_Active() ? DirectX::Colors::Yellow : false == m_isColl ? XMVectorSet(0.f, 1.f, 0.f, 1.f) : XMVectorSet(1.f, 0.f, 0.f, 1.f));
}

_bool CCollider::InterSect(CCollider* pTargetCollider)
{
	m_isColl = m_pBounding->Intersect(m_eType, pTargetCollider->m_pBounding);
	return m_isColl;
}

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}

#ifdef _DEBUG
	Safe_Release(m_pInputLayout);
#endif

	Safe_Release(m_pBounding);
}
