#include "KOUI.h"

#include "GameInstance.h"
#include "GameManager.h"

CKOUI::CKOUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CUIObject{ pDevice, pContext, ENUM_CLASS(eObjectID) }
	, m_pGameManager { CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}

CKOUI::CKOUI(const CKOUI& rhs)
	: CUIObject{ rhs }
	, m_pGameManager{ CGameManager::GetInstance() }
{
	Safe_AddRef(m_pGameManager);
}

HRESULT CKOUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKOUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CKOUI::Priority_Update(_float fTimeDelta)
{
}

void CKOUI::Update(_float fTimeDelta)
{
	if (m_bFadeIn)
		Play_Animation_FadeIn(fTimeDelta);

	if (m_bFadeOut)
		Play_Animation_FadeOut(fTimeDelta);

	/* 페이드 인 이후, 2초가 지나면 페이드 아웃 수행 */
	if (m_bTriggered)
	{
		m_fTimeAcc += fTimeDelta;

		if (m_fTimeAcc >= 2.0f)
		{
			m_fTimeAcc = 0.f;
			Start_FadeOut();
		}
	}
}

void CKOUI::Late_Update(_float fTimeDelta)
{
	if (m_bVisible)
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CKOUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CKOUI::Start_FadeIn()
{
	m_bVisible = true;
	m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT);
	m_bFadeIn = true;
	m_fFadeInTimeAcc = 0.f;

	m_bTriggered = true;
	m_fTimeAcc = 0.f;
}

void CKOUI::Start_FadeOut()
{
	m_bVisible = true;
	m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT);
	m_bFadeOut = true;
	m_fFadeOutTimeAcc = 0.f;
}

HRESULT CKOUI::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Prototype_Component_Texture_KOUI"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CKOUI::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (m_iShaderPassIdx == ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT) && m_bFadeIn)
		m_pShaderCom->Bind_Float("g_Alpha", m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc);

	else if (m_iShaderPassIdx == ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT) && m_bFadeOut)
		m_pShaderCom->Bind_Float("g_Alpha", 1 - (m_fFadeOutTimeAcc / m_fFadeOutMaxTimeAcc));

	return S_OK;
}

void CKOUI::Play_Animation_FadeIn(_float fTimeDelta)
{
	m_fFadeInTimeAcc += fTimeDelta;

	if (m_fFadeInTimeAcc > m_fFadeInMaxTimeAcc)
		m_fFadeInTimeAcc = m_fFadeInMaxTimeAcc;

	m_pTransformCom->Set_Scale(m_fSizeX * (m_fMaxScale - m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc), m_fSizeY * (m_fMaxScale - m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc), m_fZ);

	/* 애니메이션 종료 */
	if (m_fFadeInTimeAcc >= m_fFadeInMaxTimeAcc)
	{
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, m_fZ);
		m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI);
		m_bFadeIn = false;
		m_fFadeInTimeAcc = 0.f;
	}
}

void CKOUI::Play_Animation_FadeOut(_float fTimeDelta)
{
	m_fFadeOutTimeAcc += fTimeDelta;

	if (m_fFadeOutTimeAcc > m_fFadeOutMaxTimeAcc)
		m_fFadeOutTimeAcc = m_fFadeOutMaxTimeAcc;

	m_pTransformCom->Set_Scale(m_fSizeX * (m_fMaxScale * m_fFadeOutTimeAcc / m_fFadeOutMaxTimeAcc), m_fSizeY * (m_fMaxScale * m_fFadeOutTimeAcc / m_fFadeOutMaxTimeAcc), m_fZ);

	/* 애니메이션 종료 */
	if (m_fFadeOutTimeAcc >= m_fFadeOutMaxTimeAcc)
	{
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, m_fZ);
		m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI);
		m_bFadeOut = false;
		m_bVisible = false;
		m_bTriggered = false;
		m_fFadeOutTimeAcc = 0.f;
	}
}

CKOUI* CKOUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CKOUI* pInstance = new CKOUI(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : KO UI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CKOUI::Clone(void* pArg)
{
	CKOUI* pInstance = new CKOUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : KO UI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKOUI::Free()
{
	__super::Free();

	Safe_Release(m_pGameManager);
}
