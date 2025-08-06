#pragma once

#include "Client_Defines.h"
#include "Panel.h"

/*
프로그레스 바, 
회전 애니메이션 들어간 ui 하나 필요.
*/
NS_BEGIN(Client)

class CLoadingPanel final : public CPanel
{
private:
	CLoadingPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CLoadingPanel(const CLoadingPanel& rhs);
	virtual ~CLoadingPanel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Ready_LoadingUIs();

public:
	static CLoadingPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END