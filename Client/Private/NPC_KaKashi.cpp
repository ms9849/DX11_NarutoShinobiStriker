#include "NPC_KaKashi.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"
#include "Icon.h"

CNPC_KaKashi::CNPC_KaKashi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID) }
	, m_pGameManager { CGameManager::GetInstance() } 
{
	Safe_AddRef(m_pGameManager);
}

CNPC_KaKashi::CNPC_KaKashi(const CNPC_KaKashi& rhs)
	: CGameObject { rhs }
	, m_pGameManager{ CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}

HRESULT CNPC_KaKashi::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC_KaKashi::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_KaKashiIcon()))
		return E_FAIL;

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	m_pModelCom->Set_AnimIndex("KakashiNext_Idle_Loop", 1.f, true, 0.05f);
	m_eAnimState = ANIM_STATE::ANIM_IDLE;

	m_pPlayerTransform = m_pGameManager->Get_PlayerPtr()->Get_Transform();
	Safe_AddRef(m_pPlayerTransform);

	Set_Kakashi_Transform();
	Create_Dialog();

	return S_OK;
}

void CNPC_KaKashi::Priority_Update(_float fTimeDelta)
{
	Talk();
}

void CNPC_KaKashi::Update(_float fTimeDelta)
{
	if (true == m_IsActiveDeath)
	{
		m_fTimeAcc += fTimeDelta;

		if (m_fTimeAcc >= 5.f)
			m_IsDead = true;
	}

	Check_Talkable();

	_bool isAnimFinished = m_pModelCom->Play_Animation(fTimeDelta);

	if (ANIM_STATE::ANIM_GREET == m_eAnimState && true == isAnimFinished)
	{
		m_pModelCom->Set_AnimIndex("KakashiNext_Idle_Loop", 1.f, true, 0.05f);
		m_eAnimState = ANIM_STATE::ANIM_IDLE; 
	}
	
	m_pIcon->Set_Position(m_pTransformCom->Get_State(STATE::POSITION) + XMVectorSet(0.f, 2.25f, 0.f, 0.f));
}

void CNPC_KaKashi::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CNPC_KaKashi::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
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

void CNPC_KaKashi::Set_Kakashi_Transform()
{
	/* 튜토리얼 카카시 */
	if (LEVEL::TUTORIAL == m_pGameManager->Get_NextLevel())
	{
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(5.3f, 0.731f, -45.82f, 1.f));
		m_pTransformCom->Rotation(0.f, 135.f, 0.f);
	}
	/* 나뭇잎 마을 카카시 1 */
	else if (LEVEL::KONOHA_VILLAGE == m_pGameManager->Get_NextLevel() && TRIGGER_TYPE::TUTORIAL_CLEAR == m_pGameManager->Get_CurrentTrigger())
	{
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-87.878f, 17.8f, -71.192f, 1.f));
		m_pTransformCom->Rotation(0.f, 230.f, 0.f);
	}
}

void CNPC_KaKashi::Create_Dialog()
{
	if (TRIGGER_TYPE::END == m_pGameManager->Get_CurrentTrigger())
	{
		m_DialogTexts.push_back(TEXT("반갑다 하급닌자. 이 훈련장은 처음이지?"));
		m_DialogTexts.push_back(TEXT("본격적인 임무에 앞서 연습을 위한 훈련장이야"));
		m_DialogTexts.push_back(TEXT("저 앞에 보이는 푸른 원 안으로 들어가면 시작할 수 있어"));
		m_DialogTexts.push_back(TEXT("그럼 건투를 빈다"));
	}
	else if (TRIGGER_TYPE::TUTORIAL_CLEAR == m_pGameManager->Get_CurrentTrigger())
	{
		m_DialogTexts.push_back(TEXT("이봐 하급닌자, 갑작스럽지만 바로 실전 임무에 투입하게 됐다."));
		m_DialogTexts.push_back(TEXT("마을에 나타난 침입자들이 시연회를 망치려 들고 있는 상황이야.."));
		m_DialogTexts.push_back(TEXT("만약 침입자들을 무찌르지 못한다면 시연회는 완성하지 못하고\n졸작과 취업도 힘들어지겠지."));
		m_DialogTexts.push_back(TEXT("서둘러 침입자들을 무찌르고 마을의 평화를 찾아온 뒤,\n시연회를 잘 마무리 하도록!"));
	}

	m_iDialogSize = (_uint)m_DialogTexts.size();
}

void CNPC_KaKashi::Start_Dialog()
{
	/* 카메라도 NPC 전용으로 교체할 것. */
	m_pIcon->Set_Dead(true);
	m_pModelCom->Set_AnimIndex("KakashiNext_Reaction3", 1.f, true);
	m_pGameManager->Change_Camera(static_cast<LEVEL>(m_pGameInstance->Get_LevelID()), TEXT("NPC_Talk_Caemra"));
	m_pGameManager->Get_PlayerPtr()->Set_Visible(false);
	m_eAnimState = ANIM_STATE::ANIM_GREET;
	m_pGameManager->Set_Talking(true);

	m_pGameManager->Set_AttackType_Visible(false);
	m_pGameManager->Set_SkillSlot_Visible(false);

	if (TRIGGER_TYPE::END == m_pGameManager->Get_CurrentTrigger())
		m_pGameManager->OnTrigger(TRIGGER_TYPE::TUTORIAL_KAKASHI_TALK);
}

void CNPC_KaKashi::End_Dialog()
{
	m_pGameManager->Change_Camera(static_cast<LEVEL>(m_pGameInstance->Get_LevelID()), TEXT("Main_Camera"));
	m_pGameManager->Set_Dialog_Visible(false);
	m_pGameManager->Get_PlayerPtr()->Set_Visible(true);
	m_IsTalking = false;
	m_iCurrentDialog = 0;
	m_pGameManager->Set_Talking(false);

	m_pGameManager->Set_AttackType_Visible(true);
	m_pGameManager->Set_SkillSlot_Visible(true);
	m_IsActiveDeath = true;

	if (TRIGGER_TYPE::TUTORIAL_CLEAR == m_pGameManager->Get_CurrentTrigger())
		m_pGameManager->OnTrigger(TRIGGER_TYPE::KONOHA_VILLAGE_KAKASHI_TALK_1);
}

void CNPC_KaKashi::Check_Talkable()
{
	_vector vPlayerPos = m_pPlayerTransform->Get_State(STATE::POSITION);
	_float fDist = XMVectorGetX(XMVector3Length(vPlayerPos - m_pTransformCom->Get_State(STATE::POSITION)));

	/* 이미 대화중이여도 대화를 하면 안됨. */
	if (fDist <= 1.5f && false == m_IsTalking)
		m_IsTalkable = true;

	else
		m_IsTalkable = false;
}

void CNPC_KaKashi::Talk()
{
	/* F를 눌러서 대화 */
	if(m_pGameInstance->Key_Down(DIK_F) && false == m_IsTalking && true == m_IsTalkable)
	{ 
		m_IsTalking = true;
		m_iCurrentDialog = 0;

		Start_Dialog();
		m_pGameManager->Set_Dialog_Visible(true);
		m_pGameManager->Set_Dialog_Text(m_DialogTexts[m_iCurrentDialog]);
	}

	/* 이미 대화중이라면 */
	else if (m_pGameInstance->Key_Down(DIK_F) && true == m_IsTalking)
	{
		/* 대화 종료 */
		if (m_iCurrentDialog >= m_iDialogSize - 1)
			End_Dialog();
		else
		{
			m_iCurrentDialog++;
			m_pGameManager->Set_Dialog_Text(m_DialogTexts[m_iCurrentDialog]);
		}
	}
}

HRESULT CNPC_KaKashi::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_NPC_Kakashi"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_KaKashi::Ready_KaKashiIcon()
{
	CIcon::ICON_DESC Desc;
	Desc.iTextureIdx = 1;

	/* 추후 레벨 수정 */
	m_pIcon = static_cast<CIcon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(m_pGameManager->Get_NextLevel()),
		TEXT("Prototype_GameObject_Icon"), &Desc));

	Safe_AddRef(m_pIcon);
	m_pGameInstance->Add_Clone_ToLayer(m_pIcon, ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Layer_UI"));

	return S_OK;
}

HRESULT CNPC_KaKashi::Bind_ShaderResources()
{
	/*m_pShaderCom->Bind_Matrix("g_WorldMatrix", );*/
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	return S_OK;
}

CNPC_KaKashi* CNPC_KaKashi::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CNPC_KaKashi* pInstance = new CNPC_KaKashi(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : Kakashi");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CNPC_KaKashi::Clone(void* pArg)
{
	CNPC_KaKashi* pInstance = new CNPC_KaKashi(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : Kakashi");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNPC_KaKashi::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pGameManager);
	Safe_Release(m_pPlayerTransform);
}
