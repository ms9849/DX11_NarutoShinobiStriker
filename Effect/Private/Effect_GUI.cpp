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
			ImGui::InputInt("NoiseTextureNum", (_int*)&m_iNoiseTextureNum);
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
					Desc.iNoiseTextureNum = m_iNoiseTextureNum;
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

		if (ImGui::BeginTabItem("Noise"))
		{
			_int iCount = 0;
			
			for (auto& pTexture : m_NoiseSRVs)
			{
				// Image 대신 ImageButton 사용
				if (ImGui::ImageButton((string("NoiseNum") + to_string(iCount)).c_str(), (ID3D11ShaderResourceView*)pTexture, ImVec2(100.f, 100.f)))
				{
					m_iNoiseTextureNum = iCount;
				}


				if (iCount % 2 == 0)
					ImGui::SameLine(0.0f, 10.f);

				iCount++;
			}

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
		if (ImGui::BeginTabItem("Input Desc"))
		{
			/*
			_bool	m_isLoop = { false };
			_uint	m_iNumInstance = {};
			_float3 m_vCenter = {};
			_float3 m_vRange = {};
			_float3 m_vPivot = {};

			_float2 m_vSpeed = {};
			_float4 m_vColor = {};
			_float3 m_vRotation = {};
			_float2 m_vLifeTime = {};
			*/
			ImGui::Checkbox("Is Loop", &m_isLoop);
			ImGui::InputInt("Instance Number", &m_iNumInstance);

			ImGui::InputInt("Select Type", &m_iType);

			ImGui::Text("Input Center Pos		");
			
			ImGui::PushItemWidth(80.0f);

			ImGui::SameLine();
			ImGui::InputFloat("##centerx", &m_vCenter.x);
			ImGui::SameLine();
			ImGui::InputFloat("##centery", &m_vCenter.y);
			ImGui::SameLine();
			ImGui::InputFloat("##centerz", &m_vCenter.z);

			ImGui::Text("Input Range			");

			ImGui::SameLine();
			ImGui::InputFloat("##rangex", &m_vRange.x);
			ImGui::SameLine();
			ImGui::InputFloat("##rangey", &m_vRange.y);
			ImGui::SameLine();
			ImGui::InputFloat("##rangez", &m_vRange.z);

			ImGui::Text("Input Size			");

			ImGui::SameLine();
			ImGui::InputFloat("##sizex", &m_vSize.x);
			ImGui::SameLine();
			ImGui::InputFloat("##sizey", &m_vSize.y);

			ImGui::Text("Input Pivot			");

			ImGui::SameLine();
			ImGui::InputFloat("##pivotx", &m_vPivot.x);
			ImGui::SameLine();
			ImGui::InputFloat("##pivoty", &m_vPivot.y);
			ImGui::SameLine();
			ImGui::InputFloat("##pivotz", &m_vPivot.z);

			ImGui::Text("Input Speed (Min, Max) ");

			ImGui::SameLine();
			ImGui::InputFloat("##speedx", &m_vSpeed.x);
			ImGui::SameLine();
			ImGui::InputFloat("##speedy", &m_vSpeed.y);

			ImGui::Text("Input Color (RGBA)		");

			ImGui::SameLine();
			ImGui::InputFloat("##colorr", &m_vColor.x);
			ImGui::SameLine();
			ImGui::InputFloat("##colorg", &m_vColor.y);
			ImGui::SameLine();
			ImGui::InputFloat("##colorb", &m_vColor.z);
			ImGui::SameLine();
			ImGui::InputFloat("##colora", &m_vColor.w);

			ImGui::Text("Input Rotation			");

			ImGui::SameLine();
			ImGui::InputFloat("##rotationx", &m_vRotation.x);
			ImGui::SameLine();
			ImGui::InputFloat("##rotationy", &m_vRotation.y);
			ImGui::SameLine();
			ImGui::InputFloat("##rotationz", &m_vRotation.z);

			ImGui::Text("Input LifeTime			");

			ImGui::SameLine();
			ImGui::InputFloat("##lifetimex", &m_vLifeTime.x);
			ImGui::SameLine();
			ImGui::InputFloat("##lifetimey", &m_vLifeTime.y);

			ImGui::PopItemWidth();

			if (ImGui::Button("Apply"))
			{
				CParticleObject::PARTICLE_OBJECT_DESC Desc = {};
				Desc.eType = (static_cast<CParticleObject::PARTICLE_TYPE>(m_iType));
				Desc.isLoop = m_isLoop;
				Desc.iNumInstance = m_iNumInstance;
				Desc.vCenter = m_vCenter;
				Desc.vRange = m_vRange;
				Desc.vPivot = m_vPivot;
				Desc.vSize = m_vSize;
				Desc.vSpeed = m_vSpeed;
				Desc.vColor = m_vColor;
				Desc.vRotation = m_vRotation;
				Desc.vLifeTime = m_vLifeTime;
				Desc.iDiffuseTextureNum = 0; // 임시
				Desc.strDiffuseTextureTag = TEXT("Prototype_Component_Texture_Particle");
				Desc.iMaskTextureNum = 0;
				Desc.strMaskTextureTag = TEXT("Prototype_Component_Texture_Effect_Mask");

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_ParticleObject"),
					ENUM_CLASS(LEVEL::EFFECT), TEXT("Layer_Particle"), &Desc)))
				{
					MSG_BOX("Failed to Create Particle Object");
					return;
				}
			}

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
