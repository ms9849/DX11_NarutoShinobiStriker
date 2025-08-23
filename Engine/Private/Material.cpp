#include "Material.h"

#include "Shader.h"

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CMaterial::Initialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
	for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
	{
		m_iNumSRVs[i] = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(i));
		m_SRVs[i].reserve(m_iNumSRVs[i]);

		for (_uint j = 0; j < m_iNumSRVs[i]; ++j)
		{
			/*pModelFilePath : 모델파일이 저장되어있는 경로 + 모델파일이름 + 모델파일확장자. */
			/* 추출한파일의경로 + 파일이름 + 확장자 */

			aiString strTexturePath;

			_char szDrive[MAX_PATH] = {};
			_char szDir[MAX_PATH] = {};
			_char szFileName[MAX_PATH] = {};
			_char szEXT[MAX_PATH] = {};

			/* 만약 텍스쳐가 존재하지 않는다면 continue */
			if (FAILED(pAIMaterial->GetTexture(static_cast<aiTextureType>(i), j, &strTexturePath)))
				continue;

			_char szTextureFilePath[MAX_PATH] = {};
			/* 드라이브 경로 / 파일 경로 / 파일 이름 / 파일 확장자 4개로 나뉘는걸 유의할 것 */

			/* 모델 파일 경로로부터 모델의 드라이브 ,파일 경로를 가져온다. */
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

			/* 텍스쳐 파일 경로로부터 이름, 확장자를 가져온다. */
			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);
		
			strcpy_s(szTextureFilePath, szDrive);
			strcat_s(szTextureFilePath, szDir);
			strcat_s(szTextureFilePath, szFileName);
			strcat_s(szTextureFilePath, szEXT);

			_tchar szPerfectTextureFilePath[MAX_PATH] = {};
			MultiByteToWideChar(CP_ACP, 0, szTextureFilePath, (_int)strlen(szTextureFilePath),
				szPerfectTextureFilePath, MAX_PATH);

			ID3D11ShaderResourceView* pSRV = { nullptr };

			/* 텍스쳐 로딩과 동일. 확장자가 DDS냐, TGA냐, 혹은 그 외의 것이냐에 따라 처리 해줌.*/
			HRESULT hr;
			if (false == strcmp(szEXT, ".dds"))
				hr = CreateDDSTextureFromFile(m_pDevice, szPerfectTextureFilePath, nullptr, &pSRV);
			else if (false == strcmp(szEXT, ".tga"))
				hr = S_OK;
			else
				hr = CreateWICTextureFromFile(m_pDevice, szPerfectTextureFilePath, nullptr, &pSRV);

			if (FAILED(hr))
				return E_FAIL;

			m_SRVs[i].push_back(pSRV);

		}
	}

	return S_OK;
}

HRESULT CMaterial::Initialize(const _tchar* pBinaryFilePath, const IMPORT_MATERIAL_DESC& MaterialDesc)
{
	for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
	{
		m_iNumSRVs[i] = MaterialDesc.iNumSRVs[i];
		m_SRVs[i].reserve(m_iNumSRVs[i]);

		for (_uint j = 0; j < m_iNumSRVs[i]; ++j)
		{
			if (m_iNumSRVs[i] == 0)
				continue;

			_char szDrive[MAX_PATH] = {};
			_char szDir[MAX_PATH] = {};
			_char szFileName[MAX_PATH] = {};
			_char szEXT[MAX_PATH] = {};

			_char szModelFilePath[MAX_PATH] = {};
			_char szTextureFilePath[MAX_PATH] = {};

			/* 드라이브 경로 / 파일 경로 / 파일 이름 / 파일 확장자 4개로 나뉘는걸 유의할 것 */

			WideCharToMultiByte(CP_ACP, 0, pBinaryFilePath, (_int)_tcslen(pBinaryFilePath),
				szModelFilePath, MAX_PATH, NULL, NULL);

			/* 모델 파일 경로로부터 모델의 드라이브 ,파일 경로를 가져온다. */
			_splitpath_s(szModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

			/* 텍스쳐 파일 경로로부터 이름, 확장자를 가져온다. */

			_splitpath_s(&MaterialDesc.szTexturePath[i][j], nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

			strcpy_s(szTextureFilePath, szDrive);
			strcat_s(szTextureFilePath, szDir);
			strcat_s(szTextureFilePath, szFileName);
			strcat_s(szTextureFilePath, szEXT);

			_tchar szPerfectTextureFilePath[MAX_PATH] = {};
			MultiByteToWideChar(CP_ACP, 0, szTextureFilePath, (_int)strlen(szTextureFilePath),
				szPerfectTextureFilePath, MAX_PATH);

			ID3D11ShaderResourceView* pSRV = { nullptr };

			/* 텍스쳐 로딩과 동일. 확장자가 DDS냐, TGA냐, 혹은 그 외의 것이냐에 따라 처리 해줌.*/
			HRESULT hr;
			if (false == strcmp(szEXT, ".dds"))
				hr = CreateDDSTextureFromFile(m_pDevice, szPerfectTextureFilePath, nullptr, &pSRV);
			else if (false == strcmp(szEXT, ".tga"))
				hr = S_OK;
			else
				hr = CreateWICTextureFromFile(m_pDevice, szPerfectTextureFilePath, nullptr, &pSRV);

			if (FAILED(hr))
				return E_FAIL;

			m_SRVs[i].push_back(pSRV);
		}
	}


	return S_OK;
}

HRESULT CMaterial::Bind_SRV(CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iTextureIndex)
{
	/* 출력할 때도 예외처리. */
	if (iTextureIndex >= m_SRVs[eType].size())
		return S_OK;

	return pShader->Bind_SRV(pConstantName, m_SRVs[eType][iTextureIndex]);
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
	CMaterial* pInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pModelFilePath, pAIMaterial)))
	{
		MSG_BOX("Create Failed : Material By Assimp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pBinaryFilePath, const IMPORT_MATERIAL_DESC& MaterialDesc)
{
	CMaterial* pInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pBinaryFilePath, MaterialDesc)))
	{
		MSG_BOX("Create Failed : Material By Binary");
		Safe_Release(pInstance);
	}


	return pInstance;
}

void CMaterial::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (auto& SRVs : m_SRVs)
	{
		for (auto& pSRV : SRVs)
			Safe_Release(pSRV);

		SRVs.clear();
	}
}
