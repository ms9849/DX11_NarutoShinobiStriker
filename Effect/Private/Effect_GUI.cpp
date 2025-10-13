#include "Effect_GUI.h"

#include "EffectObject.h"
#include "GameInstance.h"

CEffect_GUI::CEffect_GUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice}
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEffect_GUI::Add_SRV(const _tchar* pTextureFilePath, _uint iNumTextures, TEXTURE_TYPE eTextureType)
{
	_tchar szEXT[MAX_PATH] = {};

	_wsplitpath_s(pTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

	_tchar szFullPath[MAX_PATH] = {};

	for (_uint i = 0; i < iNumTextures; ++i)
	{
		ID3D11ShaderResourceView* pSRV = { nullptr };
		HRESULT hr = {};

		wsprintf(szFullPath, pTextureFilePath, i);

		if (false == lstrcmp(szEXT, TEXT(".dds")))
			hr = CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);

		else if (false == lstrcmp(szEXT, TEXT(".tga")))
			hr = E_FAIL;

		else
			hr = CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);

		if (FAILED(hr))
			return E_FAIL;

		if (TEXTURE_TYPE::DIFFUSE == eTextureType)
			m_DiffuseSRVs.push_back(pSRV);

		else if (TEXTURE_TYPE::MASK == eTextureType)
			m_MaskSRVs.push_back(pSRV);

		else
			m_NoiseSRVs.push_back(pSRV);
	}

	return S_OK;
}

HRESULT CEffect_GUI::Add_EffeectObject(CEffectObject* pEffectObject)
{
	m_pEffectObject = pEffectObject;
	Safe_AddRef(m_pEffectObject);

	return S_OK;
}

HRESULT CEffect_GUI::Initialize()
{

	return S_OK;
}

void CEffect_GUI::Update(_float fTimeDelta)
{
	Effect_GUI();
	Particle_GUI();
}

void CEffect_GUI::Effect_GUI()
{
	ImGui::Begin("Effect Tool");

	if (ImGui::BeginTabBar("Effect"))
	{
		if (ImGui::BeginTabItem("Input"))
		{
			ImGui::InputFloat("DeltaU", &m_fDeltaU);
			ImGui::InputFloat("DeltaV", &m_fDeltaV);
			ImGui::Text("");
			ImGui::InputInt("NumWidth", (_int*)&m_iNumWidth);
			ImGui::InputInt("NumHeight", (_int*)&m_iNumHeight);
			ImGui::InputInt("Current Idx", (_int*)&m_iCurrentIdx);
			ImGui::Text("");
			ImGui::InputInt("TextureNum", (_int*)&m_iTextureNum);
			ImGui::InputInt("MaskTextureNum", (_int*)&m_iMaskTextureNum);
			ImGui::Text("");
			ImGui::InputFloat("FrameTime", &m_fFrameTime);

			if (ImGui::Button("Apply"))
			{
				if (nullptr == m_pEffectObject)
				{
					MSG_BOX("Effect Object is nullptr");
					return;
				}
				else
				{
					CEffectObject::EFFECT_OBJECT_DESC Desc = {};
					Desc.fDeltaU = m_fDeltaU;
					Desc.fDeltaV = m_fDeltaV;
					Desc.iNumWidth = m_iNumWidth;
					Desc.iNumHeight = m_iNumHeight;
					Desc.iCurrentIdx = m_iCurrentIdx;
					Desc.iTextureNum = m_iTextureNum;
					Desc.iMaskTextureNum = m_iMaskTextureNum;
					Desc.fFrameTime = m_fFrameTime;

					m_pEffectObject->Set_Desc(&Desc);
				}
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Diffuse"))
		{
			for (auto& pTexture : m_DiffuseSRVs)
				ImGui::Image((void*)pTexture, ImVec2(100, 100));

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Mask"))
		{
			for (auto& pTexture : m_MaskSRVs)	
				ImGui::Image((void*)pTexture, ImVec2(100, 100));

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Dissolve"))
		{
			for (auto& pTexture : m_NoiseSRVs)
				ImGui::Image((void*)pTexture, ImVec2(100, 100));

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void CEffect_GUI::Particle_GUI()
{
	/* 파티클은 프로토타입 입력받게? */
	ImGui::Begin("Particle Tool");

	if (ImGui::BeginTabBar("Particle"))
	{
		if (ImGui::BeginTabItem("Input"))
		{
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

CEffect_GUI* CEffect_GUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_GUI* pInstance = new CEffect_GUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Create Failed : Effect Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_GUI::Free()
{
	__super::Free();

	Safe_Release(m_pEffectObject);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (auto& pDiffuse : m_DiffuseSRVs)
		Safe_Release(pDiffuse);
	m_DiffuseSRVs.clear();

	for (auto& pMask : m_MaskSRVs)
		Safe_Release(pMask);
	m_MaskSRVs.clear();

	for (auto& pDissolve : m_NoiseSRVs)
		Safe_Release(pDissolve);
	m_NoiseSRVs.clear();

	Safe_Release(m_pGameInstance);
}
