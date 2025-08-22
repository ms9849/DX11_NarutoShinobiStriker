#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class CPooling_Manager final : public CBase
{
private:
	CPooling_Manager();
	virtual ~CPooling_Manager() = default;

public:
	map<_wstring, list<class CGameObject*>>* Get_PoolObjects(_uint iLevelID) {
		return &m_PoolObjects[iLevelID];
	}

	map<_wstring, _uint>* Get_PoolCounts(_uint iLevelID) {
		return &m_PoolCounts[iLevelID];
	}

public:
	HRESULT Initialize(_uint iNumLevels);
	void	Update();
	void	Clear(_uint iLevelIndex);
	HRESULT	Add_GameObject_ToPool(_uint iLevelIndex, class CGameObject* pGameObject);
	HRESULT	Add_PoolingObject_ToLayer(const _wstring& strPoolingTag, _uint iPoolingOjbectLevelIndex, CGameObject::GAMEOBJECT_DESC* pDesc, _uint iLayerLevelIndex, const _wstring& strLayerTag);

private:
	map<_wstring, list<class CGameObject*>>* m_PoolObjects = {};
	map<_wstring, _uint>* m_PoolCounts = {};
	_uint m_iNumLevels = {};

	class CGameInstance* m_pGameInstance = {};

public:
	static CPooling_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

NS_END
