#include "ThousandArm.h"

#include "GameManager.h"
#include "GameInstance.h"
#include "WoodArm.h"
#include "Player.h"

CThousandArm::CThousandArm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CSkill { pDevice, pContext, eObjectID }
{
}

CThousandArm::CThousandArm(const CThousandArm& rhs)
	: CSkill{ rhs }
{
}

HRESULT CThousandArm::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CThousandArm::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	m_pTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-158.f, 47.4f, 44.3f, 1.f));
	//m_IsVisible = false;

	return S_OK;
}

void CThousandArm::Priority_Update(_float fTimeDelta)
{
}

void CThousandArm::Update(_float fTimeDelta)
{
	if (true == m_IsVisible)
	{
		/* 수명 로직 */
		m_fLifeTimeAcc += fTimeDelta;
		if (m_fLifeTimeAcc >= 20.f)
		{
			m_fLifeTimeAcc = 0.f;
			m_IsActive = false;
			m_IsVisible = false;
		}

		/* 여기서 나무손 만들어서 플레이어 추적 */
		m_fWoodArmCoolDownTimeAcc += fTimeDelta;

		if (m_fWoodArmCoolDownTimeAcc >= m_fWoodArmCoolDown && true == m_IsActive)
		{
			CWoodArm::WOODARM_DESC Desc;
			Desc.fSpeedPerSec = 80.f;
			Desc.vStartPos = _float4(-158.f, 120.0f, 44.3f, 1.f);

			XMStoreFloat4(&Desc.vLookPos, CGameManager::GetInstance()->Get_PlayerPtr()->Get_Transform()->Get_State(STATE::POSITION));

			m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_WoodArm"),
				ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Layer_WoodArm"), &Desc);

			m_fWoodArmCoolDownTimeAcc = 0.f;
		}
	}
}

void CThousandArm::Late_Update(_float fTimeDelta)
{
	if (false == m_IsVisible)
		return;

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CThousandArm::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CThousandArm::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_1000Arm"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CThousandArm::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CThousandArm* CThousandArm::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CThousandArm* pInstance = new CThousandArm(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CThousandArm");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CThousandArm::Clone(void* pArg)
{
	CThousandArm* pInstance = new CThousandArm(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CThousandArm");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CThousandArm::Free()
{
	__super::Free();
}
