#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevelID);	
	virtual ~CLevel() = default;

public:
	_uint Get_LevelID() const {
		return m_iID;
	}

public:
	virtual HRESULT Initialize();
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	class CGameInstance*	m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	_uint					m_iID = {};

public:
	virtual void Free() override;
};

NS_END