#include "BossCamera.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Boss.h"

CBossCamera::CBossCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CCamera{ pDevice, pContext, ENUM_CLASS(eObjectID) }
	, m_pGameManager{ CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}

CBossCamera::CBossCamera(const CBossCamera& rhs)
	: CCamera{ rhs }
	, m_pGameManager{ CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}

HRESULT CBossCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBossCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBossCamera::Priority_Update(_float fTimeDelta)
{
	if (m_fKamuiCamTimeAcc < m_fMaxKamuiCamTimeAcc)
		m_pTransformCom->Go_Straight(fTimeDelta * m_fKamuiCamSpeedRatio);

	m_fKamuiCamTimeAcc += fTimeDelta;

	__super::Bind_Matrices();
}

void CBossCamera::Update(_float fTimeDelta)
{
}

void CBossCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CBossCamera::Render()
{
	return S_OK;
}

void CBossCamera::OnChange(const _float4x4* pWorldMatrix)
{
	m_pBossTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::KONOHA_VILLAGE), TEXT("Layer_Boss"),
		Engine::g_strTransformTag, 0));

	Safe_AddRef(m_pBossTransform);

	m_pTransformCom->Set_State(STATE::POSITION, m_pBossTransform->Get_State(STATE::POSITION) + 3.f * m_pBossTransform->Get_State(STATE::LOOK) + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
	m_pTransformCom->LookAt(m_pBossTransform->Get_State(STATE::POSITION) + XMVectorSet(0.f, 1.2f, 0.f, 0.f));
	m_fKamuiCamTimeAcc = 0.f;
}

CBossCamera* CBossCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CBossCamera* pInstance = new CBossCamera(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : CBossCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBossCamera::Clone(void* pArg)
{
	CBossCamera* pInstance = new CBossCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : CBossCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossCamera::Free()
{
	__super::Free();

	if(nullptr != m_pBossTransform)
		Safe_Release(m_pBossTransform);
	Safe_Release(m_pGameManager);
}
