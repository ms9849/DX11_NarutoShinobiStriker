#include "Particle_GUI.h"

#include "GameInstance.h"

CParticle_GUI::CParticle_GUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pGameInstance { CGameInstance::GetInstance() }
	, m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CParticle_GUI::Add_SRV(const _tchar* pTextureFilePath, _uint iNumTextures, TEXTURE_TYPE eTextureType)
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

HRESULT CParticle_GUI::Initialize()
{
	return S_OK;
}

void CParticle_GUI::Update(_float fTimeDelta)
{
	Particle_GUI();
}

void CParticle_GUI::Particle_GUI()
{
	/* 파티클은 프로토타입 입력받게? */
	ImGui::Begin("Particle Tool");

	if (ImGui::BeginTabBar("Particle"))
	{
		if (ImGui::BeginTabItem("Input Desc"))
		{

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

			ImGui::Text("Main Color (RGBA)		");

			ImGui::SameLine();
			ImGui::InputFloat("##Mcolorr", &m_vMainColor.x);
			ImGui::SameLine();
			ImGui::InputFloat("##Mcolorg", &m_vMainColor.y);
			ImGui::SameLine();
			ImGui::InputFloat("##Mcolorb", &m_vMainColor.z);
			ImGui::SameLine();
			ImGui::InputFloat("##Mcolora", &m_vMainColor.w);

			ImGui::Text("Sub Color (RGBA)		");

			ImGui::SameLine();
			ImGui::InputFloat("##Scolorr", &m_vSubColor.x);
			ImGui::SameLine();
			ImGui::InputFloat("##Scolorg", &m_vSubColor.y);
			ImGui::SameLine();
			ImGui::InputFloat("##Scolorb", &m_vSubColor.z);
			ImGui::SameLine();
			ImGui::InputFloat("##Scolora", &m_vSubColor.w);


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

			ImGui::Text("Input FrameTime		");
			ImGui::SameLine();
			ImGui::InputFloat("##frametime", &m_fFrameTime);

			ImGui::InputInt("Shader Pass", (_int*)&m_iShaderPassIdx);

			ImGui::PopItemWidth();

			/*  NoiseTexutre & Diffuse Texture & MaskTexture &...*/
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
				Desc.vRotation = m_vRotation;
				Desc.vLifeTime = m_vLifeTime;
				Desc.vMainColor = m_vMainColor;
				Desc.vSubColor = m_vSubColor;
				Desc.fFrameTime = m_fFrameTime;
				Desc.iShaderPassIdx = m_iShaderPassIdx;
				Desc.iNoiseTextureNum = m_iNoiseTextureNum;
				Desc.iDiffuseTextureNum = m_iTextureNum; // 임시
				Desc.iMaskTextureNum = m_iMaskTextureNum;

				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_ParticleObject"),
					ENUM_CLASS(LEVEL::EFFECT), TEXT("Layer_Particle"), &Desc)))
				{
					MSG_BOX("Failed to Create Particle Object");
					return;
				}
			}

			ImGui::Dummy(ImVec2(0.f, 10.f));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.f, 10.f));

			ImGui::InputText("Save Path", m_szParticleName, MAX_PATH);

			if (ImGui::Button("Save Container"))
			{
				Save_ToBinary();
			}

			ImGui::Dummy(ImVec2(0.f, 10.f));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.f, 10.f));

			ImGui::InputText("Load Path", m_szParticleLoadPath, MAX_PATH);

			if (ImGui::Button("Load Container"))
			{
				Load_FromBinary();
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
					m_iTextureNum = iCount;

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
					m_iMaskTextureNum = iCount;

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
					m_iNoiseTextureNum = iCount;

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

HRESULT CParticle_GUI::Save_ToBinary()
{
	//_char szEffectPath[MAX_PATH] = {};
	//_tchar szPerfectModelName[MAX_PATH] = {};

	//strcpy_s(szEffectPath, pEffectName);

	///* ../Bin/Resources/Fiona_eff.bin*/
	//strcat_s(szEffectPath, "_eff.bin");
	///*  char to tchar */
	//MultiByteToWideChar(CP_ACP, 0, szEffectPath, (_int)strlen(szEffectPath), szPerfectModelName, MAX_PATH);


	_char szParticlePath[MAX_PATH] = {};
	_tchar szPerfectParticleName[MAX_PATH] = {};
	strcpy_s(szParticlePath, m_szParticleName);

	/* ../Bin/Resources/Fiona_Particle.bin*/
	strcat_s(szParticlePath, "_Particle.bin");
	/*  char to tchar */
	MultiByteToWideChar(CP_ACP, 0, szParticlePath, (_int)strlen(szParticlePath), szPerfectParticleName, MAX_PATH);

	DWORD	dwByte(0);
	HANDLE hHandle = CreateFile(szPerfectParticleName,
		GENERIC_WRITE,  // 파일 용도(GENERIC_WRITE : 쓰기(저장), GENERIC_READ : 읽기(불러오기))
		NULL,			// 공유 방식(NULL인 경우 공유하지 않음)
		NULL,			// 보안 설정(NULL인 경우 기본값으로 설정)
		CREATE_ALWAYS,	// 생성 방식(CREATE_ALWAYS : 쓰기 전용, OPEN_EXISTING : 읽기 전용)
		FILE_ATTRIBUTE_NORMAL, // 파일 속성(숨김, 읽기 전용 파일 등) : 아무런 속성이 없는 일반 형식
		NULL);	// 생성될 파일의 속성을 제공할 템플릿 파일(안쓸것이기 때문에 NULL)

	if (hHandle == INVALID_HANDLE_VALUE)
		return E_FAIL;


	CParticleObject::PARTICLE_OBJECT_DESC Desc = {};
	Desc.eType = (static_cast<CParticleObject::PARTICLE_TYPE>(m_iType));
	Desc.isLoop = m_isLoop;
	Desc.iNumInstance = m_iNumInstance;
	Desc.vCenter = m_vCenter;
	Desc.vRange = m_vRange;
	Desc.vPivot = m_vPivot;
	Desc.vSize = m_vSize;
	Desc.vSpeed = m_vSpeed;
	Desc.vRotation = m_vRotation;
	Desc.vLifeTime = m_vLifeTime;
	Desc.vMainColor = m_vMainColor;
	Desc.vSubColor = m_vSubColor;
	Desc.fFrameTime = m_fFrameTime;
	Desc.iShaderPassIdx = m_iShaderPassIdx;
	Desc.iNoiseTextureNum = m_iNoiseTextureNum;
	Desc.iDiffuseTextureNum = m_iTextureNum;
	Desc.iMaskTextureNum = m_iMaskTextureNum;

	WriteFile(hHandle, &Desc, sizeof(CParticleObject::PARTICLE_OBJECT_DESC), &dwByte, nullptr);

	CloseHandle(hHandle);

	return S_OK;
}

HRESULT CParticle_GUI::Load_FromBinary()
{
	_tchar m_strParticleLoadPath[MAX_PATH] = {};
	MultiByteToWideChar(CP_ACP, 0, m_szParticleLoadPath, (_int)strlen(m_szParticleLoadPath), m_strParticleLoadPath, MAX_PATH);

	DWORD	dwByte(0);
	HANDLE hHandle = CreateFile(m_strParticleLoadPath,
		GENERIC_READ,  // 파일 용도(GENERIC_WRITE : 쓰기(저장), GENERIC_READ : 읽기(불러오기))
		FILE_SHARE_READ,			// 공유 방식(NULL인 경우 공유하지 않음)
		NULL,			// 보안 설정(NULL인 경우 기본값으로 설정)
		OPEN_EXISTING,	// 생성 방식(CREATE_ALWAYS : 쓰기 전용, OPEN_EXISTING : 읽기 전용)
		FILE_ATTRIBUTE_NORMAL, // 파일 속성(숨김, 읽기 전용 파일 등) : 아무런 속성이 없는 일반 형식
		NULL);	// 생성될 파일의 속성을 제공할 템플릿 파일(안쓸것이기 때문에 NULL)

	if (hHandle == INVALID_HANDLE_VALUE)
		return E_FAIL;

	CParticleObject::PARTICLE_OBJECT_DESC Desc = {};

	ReadFile(hHandle, &Desc, sizeof(CParticleObject::PARTICLE_OBJECT_DESC), &dwByte, nullptr);


	m_isLoop = Desc.isLoop;
	m_iType = static_cast<_int>(Desc.eType);
	m_iNumInstance = Desc.iNumInstance;
	m_vCenter = Desc.vCenter;
	m_vRange = Desc.vRange;
	m_vPivot = Desc.vPivot;
	m_vSize = Desc.vSize;
	m_vSpeed = Desc.vSpeed;
	m_vRotation = Desc.vRotation;
	m_vLifeTime = Desc.vLifeTime;
	m_vMainColor = Desc.vMainColor;
	m_vSubColor = Desc.vSubColor;
	m_fFrameTime = Desc.fFrameTime;
	m_iShaderPassIdx = Desc.iShaderPassIdx;
	m_iNoiseTextureNum = Desc.iNoiseTextureNum;
	m_iTextureNum = Desc.iDiffuseTextureNum;
	m_iMaskTextureNum = Desc.iMaskTextureNum;

	CloseHandle(hHandle);

	return S_OK;
}

CParticle_GUI* CParticle_GUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_GUI* pInstance = new CParticle_GUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Create Failed : Particle GUI");

	}
	return pInstance;
}

void CParticle_GUI::Free()
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
}
