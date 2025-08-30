#include "Player.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "SkillSlotPanel.h"
#include "AttackTypePanel.h"
#include "ComboKOPanel.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CCharacter { pDevice, pContext, eObjectID }
	, m_pGameManager{ CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CCharacter{ rhs }
	, m_pGameManager{ CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}

void CPlayer::Set_SkillSlotPanel(CSkillSlotPanel* pPanel)
{
	m_pSkillSlotPanel = pPanel;
	Safe_AddRef(m_pSkillSlotPanel);

	for (_uint i = ENUM_CLASS(SKILLNUM::SECOND); i < ENUM_CLASS(SKILLNUM::END); ++i)
		m_pSkillSlotPanel->Change_Skill(i, m_ActivatedSkills[i]);
}

void CPlayer::Set_AttackTypePanel(CAttackTypePanel* pPanel)
{
	m_pAttackTypePanel = pPanel;
	Safe_AddRef(m_pAttackTypePanel);

	m_pAttackTypePanel->Change_AttackType(m_eCurAttackType);
}

void CPlayer::Set_ComboKOPanel(CComboKOPanel* pPanel)
{
	m_pComboKOPanel = pPanel;
	Safe_AddRef(m_pComboKOPanel);
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	/* 게임 매니저에 현재 플레이어 정보 세팅. 레벨 변경되도 안전할거니까.. */
	m_pGameManager->Set_PlayerPtr(this);

	/* 플레이어는 모든 스킬을 사용할 수 있다. */
	for (_uint i = 0; i < ENUM_CLASS(SKILL::END); ++i)
		m_Skills.emplace(static_cast<SKILL>(i), g_SkillTable[i]);

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
}

void CPlayer::Update(_float fTimeDelta)
{
	/* 임시로 둔 코드. 손봐야 한다 */
	m_Skills[m_ActivatedSkills[ENUM_CLASS(SKILLNUM::SPECIAL)]].fGaugeAcc = 100.f;
	m_pSkillSlotPanel->Set_MaxSpecialSkillProgress(200.f);
	m_pSkillSlotPanel->Set_SpecialSkillProgress(100.f);

	Key_Input(fTimeDelta);
	ComboKO_System(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	/* 스킬이 바뀌었다면. */
	if (m_eCurAttackType != m_ePreAttackType)
	{
		__super::Change_AttackType();
		Change_Skills();
	}

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CPlayer::Render()
{
	if(FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Fiona"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Key_Input(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_UP))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_DOWN))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_LEFT))
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}

	if (m_pGameInstance->Key_Down(DIK_6))
	{
		m_eCurAttackType = static_cast<ATTACK_TYPE>(ENUM_CLASS(m_eCurAttackType) + 1);

		if (m_eCurAttackType == ATTACK_TYPE::END)
			m_eCurAttackType = ATTACK_TYPE::MELEE;
	}

	if (m_pGameInstance->Key_Down(DIK_7))
	{
		/* 콤보 시스템에서 초기화해줌.*/
		/* 로직은 묶였지만 아직 bool 타입 못 묶음..*/
		m_bEnemyHit = true;
	}

	if (m_pGameInstance->Key_Down(DIK_8))
	{
		/* 콤보 시스템에서 초기화해줌.*/
		/* 로직은 묶였지만 아직 bool 타입 못 묶음..*/
		m_bEnemyKO = true;
	}
}

void CPlayer::ComboKO_System(_float fTimeDelta)
{
	m_fComboTimeAcc += fTimeDelta;

	/* KO 시스템 */
	if (m_bEnemyKO)
	{
		m_pComboKOPanel->PopUp_KO();
		m_bEnemyKO = false;
	}

	/* 콤보 시스템 */
	if (m_bEnemyHit)
	{
		if(m_iComboCount < m_iMaxComboCount)
			m_iComboCount++;
		m_fComboTimeAcc = 0.f;
	}

	/* 콤보 초기화 */
	if (m_iComboCount >= 1 && m_fComboTimeAcc >= 2.f)
	{
		m_iComboCount = 0;
		m_fComboTimeAcc = 0.f;
	}

	m_pComboKOPanel->Update_Combo(m_iComboCount);
	m_bEnemyHit = false;
}

void CPlayer::Change_Skills()
{
	if (m_pSkillSlotPanel == nullptr || m_pAttackTypePanel == nullptr)
		return;

	/* 첫 번째 스킬은 그림자 분신이니까 교체 안함. */
	for (_uint i = ENUM_CLASS(SKILLNUM::SECOND); i < ENUM_CLASS(SKILLNUM::END); ++i)
		m_pSkillSlotPanel->Change_Skill(i, m_ActivatedSkills[i]);

	m_pAttackTypePanel->Change_AttackType(m_eCurAttackType);
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : PLAYER ");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : PLAYER ");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pGameManager);
	Safe_Release(m_pSkillSlotPanel);
	Safe_Release(m_pAttackTypePanel);
	Safe_Release(m_pComboKOPanel);
}
