#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrian final : public CVIBuffer
{
private:
	CVIBuffer_Terrian(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrian(const CVIBuffer_Terrian& rhs);
	virtual ~CVIBuffer_Terrian() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg) override;

private:
	_uint m_iNumVerticesX = {};
	_uint m_iNumVerticesZ = {};

public:
	static CVIBuffer_Terrian* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();
};

NS_END
