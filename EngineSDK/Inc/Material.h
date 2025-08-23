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
	HRESULT Initialize(const _tchar* pBinaryFilePath, const IMPORT_MATERIAL_DESC& MaterialDesc);
	HRESULT Bind_SRV(class CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iTextureIndex);

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	_uint								m_iNumSRVs[AI_TEXTURE_TYPE_MAX];
	vector<ID3D11ShaderResourceView*>	m_SRVs[AI_TEXTURE_TYPE_MAX];

public:
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial);
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pBinaryFilePath, const IMPORT_MATERIAL_DESC& MaterialDesc);
	virtual void Free();
};

NS_END 