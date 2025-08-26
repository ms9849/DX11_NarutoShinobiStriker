#pragma once

#include "Editor_Defines.h"
#include "Base.h"

/*
저장하고 로드할 정보들
1. Layer Props
2. Layer_Map
3. Layer StaticObject (보류)
*/

NS_BEGIN(Engine)
class CGameInstance;
NS_END 

NS_BEGIN(Editor)

class CMapConverter : public CBase 
{
private:
	CMapConverter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMapConverter() = default;

public:
	HRESULT Initialize();
	HRESULT Export_MapFiles(const _tchar* pMapName, const _tchar* pFilePath, LEVEL eLevelID);
	HRESULT Import_MapFiles(const _tchar* pFilePath, LEVEL eLevelID, LEVEL ePrototypeLevelID);

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static CMapConverter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext); 
	virtual void Free() override;
};

NS_END
