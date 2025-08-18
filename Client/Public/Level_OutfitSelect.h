#pragma once

#include "Client_Defines.h"
#include "Level.h"

/*
릴리즈 모드에서만 활성화 시키던가,
바로 넘길 수 있는 슈퍼 키가 있어야 할 것.
*/

NS_BEGIN(Client)

class CLevel_OutfitSelect final : public CLevel
{
private:
	CLevel_OutfitSelect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CLevel_OutfitSelect() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	/* 이 레벨에서 쓰기위한 객체들을 생성한다. */
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Outfits(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Mannequin(const _wstring& strLayerTag);
	HRESULT Ready_Layer_UI(const _wstring& strLayerTag);

private:
	_float m_fTimeAcc = {};
	class CGameManager* m_pGameManager = { nullptr };

public:
	static CLevel_OutfitSelect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual void Free() override;
};

NS_END