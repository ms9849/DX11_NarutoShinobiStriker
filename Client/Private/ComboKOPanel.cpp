    #include "ComboKOPanel.h"

#include "GameInstance.h"
#include "GameManager.h"

#include "Player.h"
#include "DecimalUI.h"
#include "KOUI.h"

CComboKOPanel::CComboKOPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
	: CPanel { pDevice, pContext, eObjectID }
{
}

CComboKOPanel::CComboKOPanel(const CComboKOPanel& rhs)
	: CPanel { rhs }
{
}


void CComboKOPanel::Update_Combo()
{
	/* 콤보가 갱신될때, */
	/* 콤보가 증가한다면 */

	/* 콤보 1일 때.*/
	if (m_iComboCount != m_iPreComboCount && m_iComboCount == 1)
	{
		m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT);
		m_bFadeIn = true;
		m_bVisible = true;

		m_fScale = 1.f;
		m_iTextureIdx = 0;
		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, m_fZ);
		//자식 combo들 1로 세팅
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - g_iWinSizeX / 2.f, -1.f * (m_fY - g_iWinSizeY / 2.f), m_fZ, 1.f));
	}

	/* 콤보 2 이상일 때, (Decimal도 여기서 제어 )*/
	else if (m_iComboCount != m_iPreComboCount && m_iComboCount > 1)
	{
		/* Hit, Hits 처음 나올때만 Fade In 재생 시켜준다.*/ 
		if (m_iComboCount <= 2)
		{
			m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT);
			m_bFadeIn = true;
			m_bVisible = true;
		}

		m_fScale = 0.7f;
		m_iTextureIdx = 1;
		//자식 combo들 2로 세팅
		/* 100, 50 에서 50, 20 */
		m_pTransformCom->Set_Scale(m_fSizeX * m_fScale, m_fSizeY * m_fScale, m_fZ);
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - g_iWinSizeX / 2.f + 50.f, -1.f * (m_fY - g_iWinSizeY / 2.f) + 25.f, m_fZ, 1.f));


		if (m_iComboCount >= 10)
		{
			static_cast<CDecimalUI*>(m_Childs[0])->Start_FadeIn();
			static_cast<CDecimalUI*>(m_Childs[0])->Set_CurrentIdx(m_iComboCount / 10);
		}

		static_cast<CDecimalUI*>(m_Childs[1])->Start_FadeIn();
		static_cast<CDecimalUI*>(m_Childs[1])->Set_CurrentIdx(m_iComboCount % 10);
	}

	/* 콤보가 초기화된다면 */
	else if (m_iComboCount != m_iPreComboCount && m_iComboCount == 0)
	{
		m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT);
		m_bFadeOut = true;
		m_bVisible = true;

		if (m_iPreComboCount >= 10)
		{
			for (_uint i = 0; i < 2; ++i)
				static_cast<CDecimalUI*>(m_Childs[i])->Start_FadeOut();
		}

		else if (m_iPreComboCount >= 2)
		{
			static_cast<CDecimalUI*>(m_Childs[1])->Start_FadeOut();
		}
	}

	m_iPreComboCount = m_iComboCount;
}

void CComboKOPanel::PopUp_KO()
{
	static_cast<CKOUI*>(m_Childs[2])->Start_FadeIn();
}

void CComboKOPanel::Active_Combo()
{
	m_IsEnemyHit = true;
}

void CComboKOPanel::Active_KO()
{
	m_IsEnemyKO = true;
}

HRESULT CComboKOPanel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComboKOPanel::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Deicmals()))
		return E_FAIL;

	if (FAILED(Ready_KO()))
		return E_FAIL;

	return S_OK;
}

void CComboKOPanel::Priority_Update(_float fTimeDelta)
{
}

void CComboKOPanel::Update(_float fTimeDelta)
{
	ComboKO_System(fTimeDelta);

	if (m_bFadeIn)
		Play_Animation_FadeIn(fTimeDelta);

	if (m_bFadeOut)
		Play_Animation_FadeOut(fTimeDelta);
}

void CComboKOPanel::Late_Update(_float fTimeDelta)
{
	if(m_bVisible)
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CComboKOPanel::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CComboKOPanel::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Prototype_Component_Texture_ComboKOPanel"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CComboKOPanel::Ready_Deicmals()
{
	/* Decimal 준비해줄 것 */
	for (_uint i = 0; i < 2; ++i)
	{
		CDecimalUI::DECIMAL_DESC SrcDesc;
		UIOBJECT_DESC DstDesc;

		switch (i)
		{
		case 0:
			DstDesc = CUIObject::CreateDesc(m_fX - 90, m_fY + 30, m_fZ - 0.05f, 90.f, 70.f, 0, 30.f);
			memcpy(&SrcDesc, &DstDesc, sizeof(UIOBJECT_DESC));
			SrcDesc.eDecimal = CDecimalUI::DECIMAL_TYPE::COMBO;
			break;

		case 1:
			DstDesc = CUIObject::CreateDesc(m_fX - 40, m_fY + 5, m_fZ - 0.10f, 90.f, 70.f, 0, 30.f);
			memcpy(&SrcDesc, &DstDesc, sizeof(UIOBJECT_DESC));
			SrcDesc.eDecimal = CDecimalUI::DECIMAL_TYPE::COMBO;
			break;

		default:
			break;
		}

		CDecimalUI* pDecimal = static_cast<CDecimalUI*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Prototype_GameObject_DecimalUI"), &SrcDesc));
		if (nullptr == pDecimal)
			return E_FAIL;

		pDecimal->Set_Visible(false);

		m_pGameInstance->Add_Clone_ToLayer(pDecimal, ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Layer_UI"));
		m_Childs.push_back(pDecimal);
		Safe_AddRef(pDecimal);
	}

	return S_OK;
}

HRESULT CComboKOPanel::Ready_KO()
{
	CUIObject::UIOBJECT_DESC Desc;
	Desc = CUIObject::CreateDesc(m_fX - 5, m_fY - 85, m_fZ - 0.05f, 330.f, 220.f, 0, 0.f);

	CKOUI* pKOUI = static_cast<CKOUI*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(m_pGameManager->Get_NextLevel()),
		TEXT("Prototype_GameObject_KOUI"), &Desc));

	m_pGameInstance->Add_Clone_ToLayer(pKOUI, ENUM_CLASS(m_pGameManager->Get_NextLevel()), TEXT("Layer_UI"));
	m_Childs.push_back(pKOUI);
	Safe_AddRef(pKOUI);

	return S_OK;
}

void CComboKOPanel::ComboKO_System(_float fTimeDelta)
{
	m_fComboTimeAcc += fTimeDelta;

	/* KO 시스템 */
	if (true == m_IsEnemyKO)
	{
		PopUp_KO();
		m_IsEnemyKO = false;
	}

	/* 콤보 시스템 */
	if (true == m_IsEnemyHit)
	{
		if (m_iComboCount < m_iMaxComboCount)
			m_iComboCount++;
		m_fComboTimeAcc = 0.f;
	}

	/* 콤보 초기화 */
	if (m_iComboCount >= 1 && m_fComboTimeAcc >= 5.f)
	{
		m_iComboCount = 0;
		m_fComboTimeAcc = 0.f;
	}

	Update_Combo();
	m_IsEnemyHit = false;
}

HRESULT CComboKOPanel::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (m_iShaderPassIdx == ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT) && m_bFadeIn)
	{
		_float fAlphaValue = m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc;
		m_pShaderCom->Bind_RawValue("g_Alpha", &fAlphaValue, sizeof(_float));
	}

	else if (m_iShaderPassIdx == ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI_FADEINOUT) && m_bFadeOut)
	{
		_float fAlphaValue = 1 - (m_fFadeOutTimeAcc / m_fFadeOutMaxTimeAcc);
		m_pShaderCom->Bind_RawValue("g_Alpha", &fAlphaValue, sizeof(_float));
	}

	return S_OK;
}

void CComboKOPanel::Play_Animation_FadeIn(_float fTimeDelta)
{
	m_fFadeInTimeAcc += fTimeDelta;
	
	if (m_fFadeInTimeAcc > m_fFadeInMaxTimeAcc)
		m_fFadeInTimeAcc = m_fFadeInMaxTimeAcc;

	m_pTransformCom->Set_Scale(m_fSizeX * (m_fMaxScale - m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc) * m_fScale, m_fSizeY * (m_fMaxScale - m_fFadeInTimeAcc / m_fFadeInMaxTimeAcc) * m_fScale, m_fZ);

	/* 애니메이션 종료 */
	if (m_fFadeInTimeAcc >= m_fFadeInMaxTimeAcc)
	{
		m_pTransformCom->Set_Scale(m_fSizeX * m_fScale, m_fSizeY * m_fScale, m_fZ);
		m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI);
		m_bFadeIn = false;
		m_fFadeInTimeAcc = 0.f;
	}
}

void CComboKOPanel::Play_Animation_FadeOut(_float fTimeDelta)
{
	m_fFadeOutTimeAcc += fTimeDelta;

	if (m_fFadeOutTimeAcc > m_fFadeOutMaxTimeAcc)
		m_fFadeOutTimeAcc = m_fFadeOutMaxTimeAcc;

	m_pTransformCom->Set_Scale(m_fSizeX * (m_fMaxScale * m_fFadeOutTimeAcc / m_fFadeOutMaxTimeAcc) * m_fScale, m_fSizeY * (m_fMaxScale * m_fFadeOutTimeAcc / m_fFadeOutMaxTimeAcc) * m_fScale, m_fZ);

	/* 애니메이션 종료 */
	if (m_fFadeOutTimeAcc >= m_fFadeOutMaxTimeAcc)
	{
		m_pTransformCom->Set_Scale(m_fSizeX * m_fScale, m_fSizeY * m_fScale, m_fZ);
		m_iShaderPassIdx = ENUM_CLASS(SHADER_VTXPOSTEX_IDX::UI);
		m_bFadeOut = false;
		m_bVisible = false;
		m_fFadeOutTimeAcc = 0.f;
	}
}

CComboKOPanel* CComboKOPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID)
{
	CComboKOPanel* pInstance = new CComboKOPanel(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : ComboKO panel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CComboKOPanel::Clone(void* pArg)
{
	CComboKOPanel* pInstance = new CComboKOPanel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : ComboKO panel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CComboKOPanel::Free()
{
	__super::Free();
}
