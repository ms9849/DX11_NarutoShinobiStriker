#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CMaterial final : public CBase
{
private:
	CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMaterial() = default;

public:
	HRESULT Initialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial);
	HRESULT Initialize(HANDLE hHandle, DWORD* dwByte, const _tchar* pBinaryFilePath);
	HRESULT Bind_SRV(class CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iTextureIndex);

public:
	HRESULT Save_Material_ToBinary(HANDLE hHandle, DWORD* dwByte, const aiMaterial* pAIMaterial);
	HRESULT Load_Materail_FromBinary(HANDLE hHandle, DWORD* dwByte, const _tchar* pBinaryFilePath);
private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	_uint								m_iNumSRVs[AI_TEXTURE_TYPE_MAX];
	vector<ID3D11ShaderResourceView*>	m_SRVs[AI_TEXTURE_TYPE_MAX];

public:
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial);
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HANDLE hHandle, DWORD* dwByte, const _tchar* pBinaryFilePath);
	virtual void Free();
};

NS_END 