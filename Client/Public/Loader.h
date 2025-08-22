#pragma once

#include "Client_Defines.h"
#include "Base.h"

/* 1. 다음 레벨에 대한 자원을 로드한다. */

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Loading();
	void Output();

public:
	_bool isFinished() const {
		return m_isFinished;
	}

	_float Get_LoadingProgress() {
		return m_fLoadingProgress;
	}

	_float Get_MaxLoadingProgress() {
		return m_fMaxLoadingProgress;
	}
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	LEVEL				m_eNextLevelID = { LEVEL::END };

	HANDLE				m_hThread = {};

	_wstring			m_strMessage = {};
	_bool				m_isFinished = { false };

	CRITICAL_SECTION	m_CriticalSection = {};

	CGameInstance*		m_pGameInstance = { nullptr };
	_float				m_fLoadingProgress = { 0.f };
	_float				m_fMaxLoadingProgress = { 1.f };

private:
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_OutfitSelect();
	HRESULT Loading_For_GamePlay();


public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;

};

NS_END