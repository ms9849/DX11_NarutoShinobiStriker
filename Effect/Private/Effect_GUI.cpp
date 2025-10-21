#include "Effect_GUI.h"

#include "EffectContainer.h"
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

HRESULT CEffect_GUI::Initialize()
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_EffectContainer"),
		ENUM_CLASS(LEVEL::EFFECT), TEXT("Layer_Effect"))))
		return E_FAIL;

	m_pEffectContainer = static_cast<CEffectContainer*>(m_pGameInstance->Get_GameObject(ENUM_CLASS(LEVEL::EFFECT), TEXT("Layer_Effect"), 0));

	CEffectObject::EFFECT_OBJECT_DESC Desc;
	Desc.strModelTag = Get_EffectModelTag(0);
	
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_EffectObject"),
		ENUM_CLASS(LEVEL::EFFECT), TEXT("Layer_Effect"), &Desc)))
		return E_FAIL;

	m_pEffectObject = static_cast<CEffectObject*>(m_pGameInstance->Get_GameObject(ENUM_CLASS(LEVEL::EFFECT), TEXT("Layer_Effect"), 1));

	return S_OK;
}

void CEffect_GUI::Update(_float fTimeDelta)
{
	Effect_GUI();
	Container_GUI();
}

void CEffect_GUI::Effect_GUI()
{
	ImGui::Begin("Effect Tool");

	if (ImGui::BeginTabBar("Effect"))
	{
		if (ImGui::BeginTabItem("Input"))
		{
			ImGui::Separator();

			if (ImGui::Checkbox("Visible", (_bool*)&m_IsEffectVisible))
			{
				m_pEffectObject->Set_Visible(m_IsEffectVisible);
			}

			ImGui::BeginChild("EffectMesh List", ImVec2(0, 200), true);

			for (_uint i = 0; i < m_EffectModelTags.size(); ++i)
			{
				if (ImGui::Selectable(m_pGameInstance->ToString(m_EffectModelTags[i]).c_str(), true)) {
					// 선택 동작
					m_strSelectedEffectModelTag = m_EffectModelTags[i];
					Change_Desc();
				}
			}

			ImGui::EndChild();

			ImGui::Dummy(ImVec2(0.f, 5.f));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.f, 5.f));

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
			ImGui::InputInt("Shader Pass", (_int*)&m_iShaderPassIdx);

			ImGui::PushItemWidth(60.f);

			ImGui::Text("Main Color   ");
			ImGui::SameLine();
			ImGui::InputFloat("##MColorX", &m_vMainColor.x);
			ImGui::SameLine();
			ImGui::InputFloat("##MColorY", &m_vMainColor.y);
			ImGui::SameLine();
			ImGui::InputFloat("##MColorZ", &m_vMainColor.z);
			ImGui::SameLine();
			ImGui::InputFloat("##MColorW", &m_vMainColor.w);

			ImGui::Text("Sub Color    ");
			ImGui::SameLine();
			ImGui::InputFloat("##SColorX", &m_vSubColor.x);
			ImGui::SameLine();
			ImGui::InputFloat("##SColorY", &m_vSubColor.y);
			ImGui::SameLine();
			ImGui::InputFloat("##SColorZ", &m_vSubColor.z);
			ImGui::SameLine();
			ImGui::InputFloat("##SColorW", &m_vSubColor.w);


			ImGui::Text("Rotation     ");
			ImGui::SameLine();
			ImGui::InputFloat("##RotX", &m_vEffectRotation.x);
			ImGui::SameLine();
			ImGui::InputFloat("##RotY", &m_vEffectRotation.y);
			ImGui::SameLine();
			ImGui::InputFloat("##RotZ", &m_vEffectRotation.z);

			ImGui::Text("Scale        ");
			ImGui::SameLine();
			ImGui::InputFloat("##ScaleX", &m_vScale.x);
			ImGui::SameLine();
			ImGui::InputFloat("##ScaleY", &m_vScale.y);
			ImGui::SameLine();
			ImGui::InputFloat("##ScaleZ", &m_vScale.z);

			ImGui::Text("Delta Scale  ");
			ImGui::SameLine();
			ImGui::InputFloat("##DScaleX", &m_vDeltaScale.x);
			ImGui::SameLine();
			ImGui::InputFloat("##DScaleY", &m_vDeltaScale.y);
			ImGui::SameLine();
			ImGui::InputFloat("##DScaleZ", &m_vDeltaScale.z);

			ImGui::Text("Start Time   ");
			ImGui::SameLine();
			ImGui::InputFloat("##EffStartTime", &m_fStartTime);

			ImGui::PopItemWidth();

			ImGui::Checkbox("Is Delta Rotation", &m_IsRotation);
			ImGui::Text("Rotation Speed (Degree)  ");
			ImGui::SameLine();
			ImGui::InputFloat("##DRotate", &m_fRotationPerSec);

			if (ImGui::Checkbox("Is Effect Loop", &m_IsEffectLoop)) {}

			if (ImGui::Checkbox("Is Blend", &m_IsBlend)) {}

			if (ImGui::Button("Apply"))
			{
				if (nullptr == m_pEffectObject)
				{
					MSG_BOX("Effect Object is nullptr");
					return;
				}
				else
					Change_Desc();
			}
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));


			if (ImGui::Button("Add Main To Container"))
			{
				/* Create Desc 이용해서 이펙트 객체 하나 만들어서 컨테이너에 넣어주기*/
				CEffectObject::EFFECT_OBJECT_DESC Desc = m_pEffectObject->Get_Desc();
				CEffectObject* pEffect = static_cast<CEffectObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_EffectObject"), &Desc));
				m_pEffectContainer->Add_MainEffect(pEffect);
			}

			ImGui::Dummy(ImVec2(0.f, 10.f));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.f, 10.f));

			ImGui::InputText("Effect Tag", m_szEffectContainerTag, MAX_PATH);

			if (ImGui::Button("Add Sub To Container"))
			{
				/* Create Desc 이용해서 이펙트 객체 하나 만들어서 컨테이너에 넣어주기*/
				CEffectObject::EFFECT_OBJECT_DESC Desc = m_pEffectObject->Get_Desc();
				CEffectObject* pEffect = static_cast<CEffectObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_EffectObject"), &Desc));
				m_pEffectContainer->Add_EffectObject(m_pGameInstance->ToWstring(string(m_szEffectContainerTag)), pEffect);
			}

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			ImGui::InputText("Save Effect", m_szSaveEffectName, MAX_PATH);

			if (ImGui::Button("Save"))
			{
				m_pEffectObject->Save_ToBinary(m_szSaveEffectName);
			}

			ImGui::InputText("Load Effect", m_szLoadEffectName, MAX_PATH);

			if (ImGui::Button("Load"))
			{
				m_pEffectObject->Load_FromBinary(m_pGameInstance->ToWstring(m_szLoadEffectName).c_str());
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Diffuse"))
		{
			_int iCount = 0;
			for (auto& pTexture : m_DiffuseSRVs)
			{
				// Image 대신 ImageButton 사용
				if (ImGui::ImageButton((string("DiffuseNum") + to_string(iCount)).c_str(), (ID3D11ShaderResourceView*)pTexture, ImVec2(100.f, 100.f)))
				{
					m_iTextureNum = iCount;
					Change_Desc();
				}

				if (iCount % 3 != 0 && iCount != 0)
					ImGui::SameLine(0.0f, 10.f);

				iCount++;
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Mask"))
		{
			_int iCount = 0;
			for (auto& pTexture : m_MaskSRVs)
			{
				// Image 대신 ImageButton 사용
				if (ImGui::ImageButton((string("MaskNum") + to_string(iCount)).c_str(), (ID3D11ShaderResourceView*)pTexture, ImVec2(100.f, 100.f)))
				{
					m_iMaskTextureNum = iCount;
					Change_Desc();
				}

				if (iCount % 3 != 0 && iCount != 0)
					ImGui::SameLine(0.0f, 10.f);

				iCount++;
			}

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
					Change_Desc();
				}

				if (iCount % 3 != 0 && iCount != 0)
					ImGui::SameLine(0.0f, 10.f);

				iCount++;
			}

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}

void CEffect_GUI::Container_GUI()
{
	ImGui::Begin("Container Tool");

	if (ImGui::Checkbox("Visible", (_bool*)&m_IsMainEffectVisible))
	{
		m_pEffectContainer->Set_Visible(m_IsMainEffectVisible);
	}

	if(ImGui::BeginTabBar("Container"))
	{
		if (ImGui::BeginTabItem("Effect Container"))
		{
			map<_wstring, class CEffectObject*> Effects = m_pEffectContainer->Get_Effects();

			ImGui::BeginChild("EffectMesh List", ImVec2(0, 200), true);

			for (auto& Pair : Effects)
			{
				if (ImGui::Selectable(m_pGameInstance->ToString(Pair.first).c_str()))
				{
					m_pEffectContainer->Pop(Pair.first);
				}
			}

			ImGui::EndChild();

			ImGui::EndTabItem();
		}

		/* 여기에 pop 만들어주면 됨*/

		ImGui::Dummy(ImVec2(0.f, 10.f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.f, 10.f));

		ImGui::InputText("Save Path", m_szEffectContainerSavePath, MAX_PATH);

		if (ImGui::Button("Save Container"))
		{
			m_pEffectContainer->Save_Container_ToBinary(m_szEffectContainerSavePath);
		}

		ImGui::Dummy(ImVec2(0.f, 10.f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.f, 10.f));

		ImGui::InputText("Load Path", m_szEffectContainerLoadPath, MAX_PATH);

		if (ImGui::Button("Load Container"))
		{
			m_pEffectContainer->Load_Container_FromBinary(m_pGameInstance->ToWstring(string(m_szEffectContainerLoadPath)).c_str());
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void CEffect_GUI::Add_To_Main()
{
}

void CEffect_GUI::Add_To_Sub()
{
}

void CEffect_GUI::Change_Desc()
{
	CEffectObject::EFFECT_OBJECT_DESC Desc = Create_Desc();
	m_pEffectObject->Set_Desc(&Desc);
}

CEffectObject::EFFECT_OBJECT_DESC CEffect_GUI::Create_Desc()
{
	CEffectObject::EFFECT_OBJECT_DESC Desc = {};
	Desc.strModelTag = m_strSelectedEffectModelTag;
	Desc.fDeltaU = m_fDeltaU;
	Desc.fDeltaV = m_fDeltaV;
	Desc.iNumWidth = m_iNumWidth;
	Desc.iNumHeight = m_iNumHeight;
	Desc.iCurrentIdx = m_iCurrentIdx;
	Desc.iTextureNum = m_iTextureNum;
	Desc.iMaskTextureNum = m_iMaskTextureNum;
	Desc.iNoiseTextureNum = m_iNoiseTextureNum;
	Desc.fFrameTime = m_fFrameTime;
	Desc.iShaderPassIdx = m_iShaderPassIdx;
	Desc.fLifeTime = m_fLifeTime;
	Desc.vMainColor = m_vMainColor;
	Desc.vSubColor = m_vSubColor;
	Desc.vRotation = m_vEffectRotation;
	Desc.IsRotation = m_IsRotation;
	Desc.vScale = m_vScale;
	Desc.vDeltaScale = m_vDeltaScale;
	Desc.fRotationPerSec = m_fRotationPerSec;
	Desc.fStartTime = m_fStartTime;
	Desc.IsLoop = m_IsEffectLoop;
	Desc.IsBlend = m_IsBlend;

	return Desc;
}

CEffect_GUI* CEffect_GUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_GUI* pInstance = new CEffect_GUI(pDevice, pContext);

	return pInstance;
}

void CEffect_GUI::Free()
{
	__super::Free();

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
	Safe_Release(m_pEffectObject);
}
