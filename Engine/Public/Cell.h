#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CCell : public CBase
{
private:
	CCell();
	virtual ~CCell() = default;

public:
	HRESULT Initialize(const _float3* vPoints, _uint iIndex);

private:
	_uint					m_iIndex = {};
	_float3					m_vPoints[ENUM_CLASS(NAVI_POINT::END)] = {};
	_float3					m_vNormals[ENUM_CLASS(NAVI_LINE::END)] = {};

public:
	static CCell* Create(const _float3* vPoints, _uint iIndex);
	virtual void Free();
};

NS_END
