#pragma once

#include "Client_Defines.h"
#include "Panel.h"

NS_BEGIN(Client)

class COutfitSelectPanel final : public CPanel
{
private:
	COutfitSelectPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	COutfitSelectPanel(const COutfitSelectPanel& rhs);
	virtual ~COutfitSelectPanel() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);
	void	Priority_Update(_float fTimeDelta);
	void	Update(_float fTimeDelta);
	void	Late_Update(_float fTimeDelta);
	HRESULT Render();

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Ready_CreateCharacterUI();

public:
	static COutfitSelectPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
