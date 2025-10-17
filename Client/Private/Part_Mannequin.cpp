#include "Part_Mannequin.h"

#include "GameInstance.h"

CParts_Mannequin::CParts_Mannequin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
    : CPartObject{ pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CParts_Mannequin::CParts_Mannequin(const CParts_Mannequin& rhs)
    : CPartObject{ rhs }
{
}

void CParts_Mannequin::Set_AnimIndex(const _char* pAnimName, _float fAnimationPlayRate, _bool IsBlend, _float fBlendRatio, _bool IsLoop)
{
    /* 추후 문자열로 수정해야 함. */
    if (nullptr != m_pModelCom)
        m_pModelCom->Set_AnimIndex(pAnimName, fAnimationPlayRate, IsBlend, fBlendRatio, IsLoop);
}

_bool CParts_Mannequin::Play_Animation(_float fTimeDelta)
{
    return m_pModelCom->Play_Animation(fTimeDelta);
}

HRESULT CParts_Mannequin::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParts_Mannequin::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

	m_strModelName = static_cast<MANNEQUIN_PART_DESC*>(pArg)->strModelName;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex("CustomMan_Idle_Loop", 1.f, false, 0.1f);
	m_pModelCom->Play_Animation(0.f);

    return S_OK;
}

void CParts_Mannequin::Priority_Update(_float fTimeDelta)
{
}

void CParts_Mannequin::Update(_float fTimeDelta)
{
	/* 부모 행렬 적용 */
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
}

void CParts_Mannequin::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, this);
}

HRESULT CParts_Mannequin::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CParts_Mannequin::Ready_Components()
{
	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), m_strModelName,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParts_Mannequin::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CParts_Mannequin* CParts_Mannequin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CParts_Mannequin* pInstance = new CParts_Mannequin(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create failed : Part Mannequin");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CParts_Mannequin::Clone(void* pArg)
{
	CParts_Mannequin* pInstance = new CParts_Mannequin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone failed : Part Mannequin");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CParts_Mannequin::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

}
