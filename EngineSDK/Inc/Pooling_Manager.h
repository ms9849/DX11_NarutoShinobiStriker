#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CPooling_Manager final : public CBase
{
private:
	CPooling_Manager();
	virtual ~CPooling_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT	Add_GameObject_ToPool(class CGameObject* pGameObject);
	HRESULT	Add_PoolingObject_ToLayer(const _wstring& strPoolingTag, _uint iLayerLevelIndex, const _wstring& strLayerTag);

private:
	map<_wstring, list<class CGameObject*>> m_PoolObjects = {};
	class CGameInstance* m_pGameInstance = {};

public:
	static CPooling_Manager* Create();
	virtual void Free() override;
};

NS_END
