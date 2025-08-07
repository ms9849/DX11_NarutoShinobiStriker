#pragma once

#include "Client_Defines.h"
#include "Panel.h"

NS_BEGIN(Client)

class CLoadingBarPanel final : public CPanel
{
private:
	CLoadingBarPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CLoadingBarPanel(const CLoadingBarPanel& rhs);
	virtual ~CLoadingBarPanel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_LoadingProgress(_float fProgress);
	void Set_MaxLoadingProgress(_float fMaxProgress);

private:
	HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources() override;
	HRESULT Ready_ProgressBar();

public:
	static CLoadingBarPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


NS_END 
