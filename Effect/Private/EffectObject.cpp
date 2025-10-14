#include "EffectObject.h"

#include "GameInstance.h"

CEffectObject::CEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID)
    : CGameObject { pDevice, pContext, ENUM_CLASS(eObjectID)}
{
}

CEffectObject::CEffectObject(const CEffectObject& rhs) 
    : CGameObject { rhs }
{
}

HRESULT CEffectObject::Save_ToBinary(const _char* pEffectName)
{
	_char szEffectPath[MAX_PATH] = {};
	_tchar szPerfectModelName[MAX_PATH] = {};

	strcpy_s(szEffectPath, pEffectName);
		
	/* ../Bin/Resources/Fiona_eff.bin*/
	strcat_s(szEffectPath, "_eff.bin");
	/*  char to tchar */
	MultiByteToWideChar(CP_ACP, 0, szEffectPath, (_int)strlen(szEffectPath), szPerfectModelName, MAX_PATH);

	DWORD	dwByte(0);
	HANDLE hHandle = CreateFile(szPerfectModelName,
		GENERIC_WRITE,  // 파일 용도(GENERIC_WRITE : 쓰기(저장), GENERIC_READ : 읽기(불러오기))
		NULL,			// 공유 방식(NULL인 경우 공유하지 않음)
		NULL,			// 보안 설정(NULL인 경우 기본값으로 설정)
		CREATE_ALWAYS,	// 생성 방식(CREATE_ALWAYS : 쓰기 전용, OPEN_EXISTING : 읽기 전용)
		FILE_ATTRIBUTE_NORMAL, // 파일 속성(숨김, 읽기 전용 파일 등) : 아무런 속성이 없는 일반 형식
		NULL);	// 생성될 파일의 속성을 제공할 템플릿 파일(안쓸것이기 때문에 NULL)

	if (hHandle == INVALID_HANDLE_VALUE)
		return E_FAIL;

	_char* pExt = strrchr(szEffectPath, '_'); 
	if (pExt)
		*pExt = '\0';

	/* 모델 태그 */
	size_t StringSize = m_strModelName.size();
	WriteFile(hHandle, &StringSize, sizeof(size_t), &dwByte, nullptr);
	WriteFile(hHandle, m_strModelName.c_str(), static_cast<DWORD>(m_strModelName.size() * sizeof(wchar_t)), &dwByte, NULL);

	/* 디퓨즈 텍스쳐 넘버*/
	WriteFile(hHandle, &m_iDiffuseTextureIdx, sizeof(_uint), &dwByte, nullptr);

	/* 마스크 텍스쳐 넘버 */
	WriteFile(hHandle, &m_iMaskTextureIdx, sizeof(_uint), &dwByte, nullptr);

	/* 노이즈 텍스쳐 넘버 */
	WriteFile(hHandle, &m_iNoiseTextureIdx, sizeof(_uint), &dwByte, nullptr);

	/* U,V 델타 */
	WriteFile(hHandle, &m_fDeltaU, sizeof(_float), &dwByte, nullptr);
	WriteFile(hHandle, &m_fDeltaV, sizeof(_float), &dwByte, nullptr);

	/* 가로 세로 프레임 갯수 */
	WriteFile(hHandle, &m_iNumWidth, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hHandle, &m_iNumHeight, sizeof(_uint), &dwByte, nullptr);

	/* 최대 인덱스 */
	WriteFile(hHandle, &m_iMaxIdx, sizeof(_uint), &dwByte, nullptr);

	/* 프레임 타임 */
	WriteFile(hHandle, &m_fFrameTime, sizeof(_float), &dwByte, nullptr);

	/* 라이프 타임 */
	WriteFile(hHandle, &m_fLifeTime, sizeof(_float), &dwByte, nullptr);

	CloseHandle(hHandle);
}

HRESULT CEffectObject::Load_FromBinary(const _tchar* pBinaryFilePath)
{
	/* ../Bin/Resources/Models/Binary/Fiona_eff.bin */
	_char szEffectFilePath[MAX_PATH];

	WideCharToMultiByte(CP_ACP, 0, pBinaryFilePath, (_int)_tcslen(pBinaryFilePath), szEffectFilePath, MAX_PATH, NULL, NULL);

	_char szEffectPath[MAX_PATH] = {};


	DWORD	dwByte(0);
	HANDLE hHandle = CreateFile(pBinaryFilePath,
		GENERIC_READ,  // 파일 용도(GENERIC_WRITE : 쓰기(저장), GENERIC_READ : 읽기(불러오기))
		NULL,			// 공유 방식(NULL인 경우 공유하지 않음)
		NULL,			// 보안 설정(NULL인 경우 기본값으로 설정)
		OPEN_EXISTING,	// 생성 방식(CREATE_ALWAYS : 쓰기 전용, OPEN_EXISTING : 읽기 전용)
		FILE_ATTRIBUTE_NORMAL, // 파일 속성(숨김, 읽기 전용 파일 등) : 아무런 속성이 없는 일반 형식
		NULL);	// 생성될 파일의 속성을 제공할 템플릿 파일(안쓸것이기 때문에 NULL)

	if (hHandle == INVALID_HANDLE_VALUE)
		return E_FAIL;

	/* 모델 태그 */
	size_t StringSize = {};
	ReadFile(hHandle, &StringSize, sizeof(size_t), &dwByte, nullptr);
	m_strModelName.resize(StringSize);

	ReadFile(hHandle, &m_strModelName[0], static_cast<DWORD>(m_strModelName.size() * sizeof(wchar_t)), &dwByte, NULL);

	/* 디퓨즈 텍스쳐 넘버*/
	ReadFile(hHandle, &m_iDiffuseTextureIdx, sizeof(_uint), &dwByte, nullptr);

	/* 마스크 텍스쳐 넘버 */
	ReadFile(hHandle, &m_iMaskTextureIdx, sizeof(_uint), &dwByte, nullptr);

	/* 노이즈 텍스쳐 넘버 */
	ReadFile(hHandle, &m_iNoiseTextureIdx, sizeof(_uint), &dwByte, nullptr);

	/* U,V 델타 */
	ReadFile(hHandle, &m_fDeltaU, sizeof(_float), &dwByte, nullptr);
	ReadFile(hHandle, &m_fDeltaV, sizeof(_float), &dwByte, nullptr);

	/* 가로 세로 프레임 갯수 */
	ReadFile(hHandle, &m_iNumWidth, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hHandle, &m_iNumHeight, sizeof(_uint), &dwByte, nullptr);

	/* 최대 인덱스 */
	ReadFile(hHandle, &m_iMaxIdx, sizeof(_uint), &dwByte, nullptr);

	/* 프레임 타임 */
	ReadFile(hHandle, &m_fFrameTime, sizeof(_float), &dwByte, nullptr);

	/* 라이프 타임 */
	ReadFile(hHandle, &m_fLifeTime, sizeof(_float), &dwByte, nullptr);
}

HRESULT CEffectObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffectObject::Initialize(void* pArg)
{
	EFFECT_OBJECT_DESC* pDesc = static_cast<EFFECT_OBJECT_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

	/* IsBinary가 True라면 여기서 다르게 세팅*/

    if (FAILED(Ready_Components(pDesc->strModelTag)))
        return E_FAIL;

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

    return S_OK;
}

void CEffectObject::Priority_Update(_float fTimeDelta)
{
}

void CEffectObject::Update(_float fTimeDelta)
{
	if (false == m_IsVisible)
		return;

	Play_Sprite(fTimeDelta);
	Check_LifeTime(fTimeDelta);
}

void CEffectObject::Late_Update(_float fTimeDelta)
{
	if (false == m_IsVisible)
		return;

    /* 문제없나..?*/
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CEffectObject::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_iNumWidth", &m_iNumWidth, sizeof(_uint))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_iNumHeight", &m_iNumWidth, sizeof(_uint))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDeltaU", &m_fDeltaU, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDeltaV", &m_fDeltaV, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_iCurrentIdx", &m_iCurrentIdx, sizeof(_uint))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fLifeTimeAcc", &m_fLifeTimeAcc, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fLifeTime", &m_fLifeTime, sizeof(_float))))
			return E_FAIL;

		/* 바인드 머테리얼이 아니라 내가 들고 있는 텍스쳐를 선택해서 바인딩 해줘야지. */
		if (FAILED(m_pDiffuseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iDiffuseTextureIdx)))
			return E_FAIL;

		if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_iMaskTextureIdx)))
			return E_FAIL;

		if (FAILED(m_pNoiseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", m_iNoiseTextureIdx)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CEffectObject::Play_Sprite(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc >= m_fFrameTime)
	{
		m_fTimeAcc = 0.f;
		m_iCurrentIdx++;

		if (m_iCurrentIdx >= m_iMaxIdx)
			m_iCurrentIdx = 0;
	}
}

void CEffectObject::Check_LifeTime(_float fTimeDelta)
{
	m_fLifeTimeAcc += fTimeDelta;

	if (m_fLifeTimeAcc >= m_fLifeTime)
	{
		m_fLifeTimeAcc = 0.f;
		//사망처리 해줘야됨 원래
	}
}

void CEffectObject::Set_Desc(void* pArg)
{
	EFFECT_OBJECT_DESC* pDesc = static_cast<EFFECT_OBJECT_DESC*>(pArg);
	if (TEXT("") != pDesc->strModelTag)
	{
		Safe_Release(m_pModelCom);
		m_pModelCom = { nullptr };

		auto iter = m_Components.find(TEXT("Com_Model"));
		Safe_Release(iter->second);
		m_Components.erase(iter);

		__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), pDesc->strModelTag,
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom));
		m_strModelName = pDesc->strModelTag;
		m_iNumMeshes = m_pModelCom->Get_NumMeshes();
	}

	if(-1 != pDesc->iNumHeight)
		m_iNumHeight = pDesc->iNumHeight;

	if (-1 != pDesc->iNumWidth)
		m_iNumWidth = pDesc->iNumWidth;

	if (-1.f != pDesc->fDeltaU)
		m_fDeltaU = pDesc->fDeltaU;

	if (-1.f != pDesc->fDeltaV)
		m_fDeltaV = pDesc->fDeltaV;

	if (-1 != pDesc->iCurrentIdx)
		m_iCurrentIdx = pDesc->iCurrentIdx;

	if (-1.f != pDesc->fFrameTime)
		m_fFrameTime = pDesc->fFrameTime;
	else
		m_fFrameTime = 0.1f;

	m_fLifeTime = pDesc->fFrameTime * m_iNumHeight * m_iNumWidth;

	if (m_iNumHeight * m_iNumWidth > 0)
		m_iMaxIdx = m_iNumHeight * m_iNumWidth - 1;

	else
		m_iMaxIdx = 0;

	if(-1 != pDesc->iTextureNum)
		m_iDiffuseTextureIdx = pDesc->iTextureNum;

	if (-1 != pDesc->iMaskTextureNum)
		m_iMaskTextureIdx = pDesc->iMaskTextureNum;

	if (-1 != pDesc->iNoiseTextureNum)
		m_iNoiseTextureIdx = pDesc->iNoiseTextureNum;

	/* Com_DiffuseTexture */
	if (TEXT("") != pDesc->strTextureTag)
	{
		auto iter = m_Components.find(TEXT("Com_DiffuseTexture"));
		Safe_Release(iter->second);
		m_Components.erase(iter);

		Safe_Release(m_pDiffuseTextureCom);
		m_pDiffuseTextureCom = { nullptr };

		__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), pDesc->strTextureTag,
			TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom));
	}

	/* Com_MaskTexture */
	if (TEXT("") != pDesc->strMaskTextureTag)
	{
		auto iter = m_Components.find(TEXT("Com_MaskTexture"));
		Safe_Release(iter->second);
		m_Components.erase(iter);

		Safe_Release(m_pMaskTextureCom);
		m_pMaskTextureCom = { nullptr };

		__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), pDesc->strMaskTextureTag,
			TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom));
	}
}

HRESULT CEffectObject::Ready_Components(const _wstring& strModelTag)
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Shader_VtxEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_DiffuseTexture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Effect_Diffuse"),
		TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pDiffuseTextureCom))))
		return E_FAIL;

	/* Com_MaskTexture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Effect_Mask"),
		TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
		return E_FAIL;

	/* Com_NoiseTexture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Effect_Noise"),
		TEXT("Com_NoiseTexture"), reinterpret_cast<CComponent**>(&m_pNoiseTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectObject::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_PipeLine_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CEffectObject* CEffectObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Client::OBJECTID eObjectID)
{
	CEffectObject* pInstance = new CEffectObject(pDevice, pContext, eObjectID);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : EffectObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffectObject::Clone(void* pArg)
{
	CEffectObject* pInstance = new CEffectObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CEffectObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectObject::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pDiffuseTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pNoiseTextureCom);
}
