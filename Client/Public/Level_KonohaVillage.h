#pragma once

#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)

class CLevel_KonohaVillage final : public CLevel
{
private:
	CLevel_KonohaVillage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CLevel_KonohaVillage() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	// 시연회 끝나면 삭제
	HRESULT Ready_Temp();
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const _wstring& strLayerTag);
	HRESULT Ready_Layer_UI(const _wstring& strLayerTag);
	HRESULT Ready_Layer_StaticObjects(const _wstring& strLayerTag);
	HRESULT Ready_Layer_NPC(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Map(const _wstring& strLayerTag);

private:
	class CLight_GUI* m_pLightGUI = { nullptr };
	class CGameManager* m_pGameManager = { nullptr };
	_float				m_fTimeAcc = { 0.f };

public:
	static CLevel_KonohaVillage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual void Free() override;
};

NS_END