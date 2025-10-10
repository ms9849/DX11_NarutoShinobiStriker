#include "Icon.h"
#include "GameManager.h"
#include "GameInstance.h"

#include "Effect_Icon.h"

CIcon::CIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CIcon::CIcon(const CIcon& rhs) 
	: CGameObject{ rhs }
{
}

void CIcon::Set_Position(_fvector vPos)
{
	m_pTransformCom->Set_State(STATE::POSITION, vPos);
}

HRESULT CIcon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CIcon::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	ICON_DESC* pDesc = static_cast<ICON_DESC*>(pArg);
	m_iTextureIdx = pDesc->iTextureIdx;

	/* 행렬 저장해. */
	m_pOrthoTransform = CTransform::Create(m_pDevice, m_pContext);
	m_pOrthoTransform->Initialize(nullptr);
	m_Components.emplace(TEXT("Com_OrthoTransform"), m_pOrthoTransform);
	Safe_AddRef(m_pOrthoTransform);

	return S_OK;
}

void CIcon::Priority_Update(_float fTimeDelta)
{
}

void CIcon::Update(_float fTimeDelta)
{
}

void CIcon::Late_Update(_float fTimeDelta)
{
	if (true == m_pGameInstance->IsInViewPort(m_pTransformCom->Get_State(STATE::POSITION), 1.f, 1.f, &m_fOrthoX, &m_fOrthoY))
	{
		m_IsOrthogonal = false;
		m_fTimeAcc += fTimeDelta;

		if (m_fTimeAcc >= 1.0f)
		{
			CEffect_Icon::EFFECT_ICON_DESC Desc;
			Desc.pTargetTransform = m_pTransformCom;

			m_pGameInstance->Add_PoolingObject_ToLayer(TEXT("Effect_Icon"), m_pGameInstance->Get_LevelID(),
				&Desc, m_pGameInstance->Get_LevelID(), TEXT("Layer_Effect"));

			m_fTimeAcc = 0.f;
		}

		m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
	}
	else
	{
		m_IsOrthogonal = true;
		m_pGameInstance->Add_RenderGroup(RENDER::WORLD_UI, this);
	}
}

HRESULT CIcon::Render()
{
	_float fScreenX, fScreenY;
	/* 카메라랑 같은 시점 바라보게끔 설정 */
	if (false == m_IsOrthogonal)
	{
		_float4x4 CameraWorld = *m_pGameInstance->Get_PipeLine_InverseFloat4x4(D3DTS::VIEW);

		m_pTransformCom->Set_State(STATE::RIGHT, *reinterpret_cast<_vector*>(&CameraWorld.m[0]));
		m_pTransformCom->Set_State(STATE::UP, *reinterpret_cast<_vector*>(&CameraWorld.m[1]));
		m_pTransformCom->Set_State(STATE::LOOK, *reinterpret_cast<_vector*>(&CameraWorld.m[2]));
		/* 거리에 따른 크기 축소..? */
		m_pTransformCom->Set_Scale(0.8f, 0.8f, 0.8f);

		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

	}
	/* 직교로 투영 */
	else
	{
		m_pOrthoTransform->Set_Scale(75.f, 75.f, 1.f);
		m_pOrthoTransform->Set_State(STATE::POSITION, XMVectorSet(m_fOrthoX - g_iWinSizeX / 2.f, g_iWinSizeY / 2.f - m_fOrthoY, 0.02f, 1.f));

		if (FAILED(Bind_Orthogonal_ShaderResources()))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CIcon::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Icon"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Orthogonal"),
		TEXT("Com_Orthogonal"), reinterpret_cast<CComponent**>(&m_pOrthogonalCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CIcon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIdx)))
		return E_FAIL;

	return S_OK;
}

HRESULT CIcon::Bind_Orthogonal_ShaderResources()
{
	if (FAILED(m_pOrthoTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pOrthogonalCom->Bind_ViewMatrix(m_pShaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_pOrthogonalCom->Bind_ProjMatrix(m_pShaderCom, "g_ProjMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIdx)))
		return E_FAIL;

	return S_OK;
}

CIcon* CIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CIcon* pInstance = new CIcon(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : CIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CIcon* CIcon::Clone(void* pArg)
{
	CIcon* pInstance = new CIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : CIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIcon::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pOrthoTransform);
	Safe_Release(m_pOrthogonalCom);
}
