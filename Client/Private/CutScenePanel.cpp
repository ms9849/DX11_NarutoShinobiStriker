#include "CutScenePanel.h"

#include "GameInstance.h"
#include "GameManager.h"


CCutScenePanel::CCutScenePanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CPanel { pDevice, pContext, eObjectID }
{
}

CCutScenePanel::CCutScenePanel(const CCutScenePanel& rhs)
	: CPanel { rhs }
{
}

HRESULT CCutScenePanel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCutScenePanel::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CCutScenePanel::Priority_Update(_float fTimeDelta)
{
}

void CCutScenePanel::Update(_float fTimeDelta)
{
	TRIGGER_TYPE eType = m_pGameManager->Get_CurrentTrigger();

	switch (eType)
	{
	case TRIGGER_TYPE::TUTORIAL_CLEAR:
		m_strFontText = TEXT("나뭇잎 마을");
		break;

	case TRIGGER_TYPE::KONOHA_VILLAGE_SPAWNER_BOSS:
		m_strFontText = TEXT("CUTSCENE");
		m_strScriptFontText = TEXT("(boss) 네 놈의 시연회는 여기까지다!!");
		m_IsActiveScript = true;
		break;

	default:
		m_strFontText = TEXT("중급 닌자 시험장");
		break;
	}

}

void CCutScenePanel::Late_Update(_float fTimeDelta)
{
	if (false == m_IsVisible)
		return;

	/* 렌더러에 자기자신 추가 */
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);

	m_pFontCom->Bind_Resources(m_strFontText.c_str(), _float2{ -1.f * g_iWinSizeX / 2.f + 30.f, g_iWinSizeY / 2.f - 30.f }, false,
		1.f, XMVectorSet(1.f, 1.f, 1.f, 1.f));

	m_pGameInstance->Add_Font(m_pFontCom);

	if (true == m_IsActiveScript)
	{
		m_pScriptFontCom->Bind_Resources(m_strScriptFontText.c_str(), _float2{ 0.f, -287.f }, true,
			0.6f, XMVectorSet(1.f, 1.f, 1.f, 1.f));

		m_pGameInstance->Add_Font(m_pScriptFontCom);
	}
}

HRESULT CCutScenePanel::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCutScenePanel::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_CutSceneUI"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Font"),
		TEXT("Com_Font"), reinterpret_cast<CComponent**>(&m_pFontCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Font"),
		TEXT("Com_Font_Script"), reinterpret_cast<CComponent**>(&m_pScriptFontCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCutScenePanel::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

CCutScenePanel* CCutScenePanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CCutScenePanel* pInstance = new CCutScenePanel(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Faield : CutScene Panel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCutScenePanel::Clone(void* pArg)
{
	CCutScenePanel* pInstance = new CCutScenePanel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Faield : CutScene Panel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCutScenePanel::Free()
{
	__super::Free();

	Safe_Release(m_pFontCom);
	Safe_Release(m_pScriptFontCom);
}
