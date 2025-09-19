#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "Model.h"
//#include "VIBuffer_Cube.h"
#include "VIBuffer_Rect_Instance.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"
#include "Orthogonal.h"
#include "Font.h"
#include "Navigation.h"
#include "Collider.h"

/* 1. 객체들의 원형을 레벨별로 보관한다. */
/* 1. 컴포넌트들의 원형을 레벨별로 보관한다. */

NS_BEGIN(Engine)

class CPrototype_Manager final : public CBase
{
private:
	CPrototype_Manager();
	virtual ~CPrototype_Manager() = default;

public:
	map<const _wstring, class CBase*>* Get_Prototypes(_uint iLevelID);

public:
	class CBase* Get_Prototype(_uint iLevelID, const _wstring& strPrototypeTag);

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CBase* pPrototype);
	class CBase* Clone_Prototype(PROTOTYPE ePrototype, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg);
	void Clear(_uint iLevelIndex);

private:
	_uint										m_iNumLevels = { };
	map<const _wstring, class CBase*>*			m_pPrototypes = { nullptr };

private:
	class CBase* Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag);

public:
	static CPrototype_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

NS_END