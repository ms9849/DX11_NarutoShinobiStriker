#include "Character.h"
#include "GameInstance.h"

CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CContainerObject{ pDevice, pContext, ENUM_CLASS(eObjectID) }
{
}

CCharacter::CCharacter(const CCharacter& rhs)
	: CContainerObject{ rhs }
{
}

HRESULT CCharacter::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCharacter::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/* 0번은 그림자 바꿔치기로 확정. */
	m_ActivatedSkills[ENUM_CLASS(SKILLNUM::FIRST)] = SKILL::SHADOW_ESCAPE;

	Change_AttackType();

	return S_OK;
}

void CCharacter::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CCharacter::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CCharacter::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CCharacter::Render()
{
	//if (FAILED(Bind_ShaderResources()))
	//	return E_FAIL;

	//for (_uint i = 0; i < m_iNumMeshes; ++i)
	//{
	//	if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0)))
	//		return E_FAIL;

	//	if (FAILED(m_pShaderCom->Begin(0)))
	//		return E_FAIL;

	//	if (FAILED(m_pModelCom->Render(i)))
	//		return E_FAIL;
	//}

	return S_OK;
}

void CCharacter::Change_AttackType()
{
	if (m_ePreAttackType != m_eCurAttackType)
	{
		/* 여기서 스킬 타입에 따라 스킬들 변경시켜준다.*/
		/* Late_Update 단에서 수행해줌으로서 안전한 상태 변환을 보장해줌.*/
		/* 정말 필요하다면 FSM으로 관리해도 괜찮겠지만.. */
		/* FSM은 모든 상태를 클래스화 해야 하므로 작성하는데 시간이 걸리고, */
		/* 메모리를 잡아먹는다는 단점이 있음 */
		switch (m_eCurAttackType)
		{
		case ATTACK_TYPE::MELEE:
			m_ActivatedSkills[1] = SKILL::RASENGAN;
			m_ActivatedSkills[2] = SKILL::RASEN_SHURIKEN;
			m_ActivatedSkills[3] = SKILL::BIG_RASENGAN;
			break;

		case ATTACK_TYPE::NINJUTSU:
			m_ActivatedSkills[1] = SKILL::CHIDORI;
			m_ActivatedSkills[2] = SKILL::FIREBALL;
			m_ActivatedSkills[3] = SKILL::BIG_SHARK;
			break;

		//case ATTACK_TYPE::DEFENSIVE:
		//	m_ActivatedSkills[1] = SKILL::WOOD_WALL;
		//	m_ActivatedSkills[2] = SKILL::WOOD_HAND;
		//	m_ActivatedSkills[3] = SKILL::KAMUI;
		//	break;

		default:
			break;
		}

		m_ePreAttackType = m_eCurAttackType;
	}
}

HRESULT CCharacter::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
	//	return E_FAIL;

	return S_OK;
}

void CCharacter::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
