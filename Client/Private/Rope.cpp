#include "Rope.h"

#include "GameManager.h"
#include "GameInstance.h"
#include "Player.h"

CRope::CRope(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID) 
	: CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
	, m_pGameManager { CGameManager::GetInstance()}
{
	Safe_AddRef(m_pGameManager);
}

CRope::CRope(const CRope& rhs)
	: CGameObject { rhs }
	, m_pGameManager{ CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}

HRESULT CRope::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRope::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	ROPE_DESC* pDesc = static_cast<ROPE_DESC*>(pArg);
	pDesc->fSpeedPerSec = 40.f;


	/* 손 본의 컴바인드 매트릭스를 가져오게 하자.*/
	/* 그리고 start pos로 세팅, end pos는 현재 로프의 월드 위치로 세팅.*/
	/* 그걸 지오메트리에서 그려낸다. */

	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&pDesc->vStartPos));
	m_vTargetPos = pDesc->vTargetPos;
	/* 방향은 타겟 위치에서 내 위치 뺴서 구해줄 것 */
	XMStoreFloat3(&m_vTargetDir, XMVector3Normalize(XMLoadFloat3(&pDesc->vTargetPos) - m_pTransformCom->Get_State(STATE::POSITION)));

	m_pTransformCom->LookAt(XMLoadFloat3(&m_vTargetPos));
	m_pTransformCom->Set_Scale(0.3f, 0.3f, 0.3f); 

	return S_OK;
}

void CRope::Priority_Update(_float fTimeDelta)
{
}

void CRope::Update(_float fTimeDelta)
{
	if(false == m_IsArrive)
		m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) + XMLoadFloat3(&m_vTargetDir) * fTimeDelta * 60.f);

	_float fDist = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(STATE::POSITION) - XMLoadFloat3(&m_vTargetPos)));
	
	if (fDist < 0.5f)
		m_IsArrive = true;

	m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CRope::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
	m_pGameInstance->Add_RenderGroup(RENDER::BLUR, this);
}

HRESULT CRope::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_Texture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX_IDX::ROPE_TRAIL))))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

HRESULT CRope::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_SkyBox"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_Sphere::BOUNDING_SPHERE_DESC		SphereDesc{};

	SphereDesc.fRadius = 0.5f;
	SphereDesc.vCenter = { 0.f, 0.f, 0.f };

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SphereDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRope::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_RopeSocketMatrix", m_pGameManager->Get_PlayerPtr()->Get_CombinedMatrix(TEXT("Part_Upper"), "RightHandMiddle1"))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_RopeWorldMatrix", m_pTransformCom->Get_WorldMatrixPtr())))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CRope* CRope::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CRope* pInstance = new CRope(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRope");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRope::Clone(void* pArg)
{
	CRope* pInstance = new CRope(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CRope");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRope::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pGameManager);
}
