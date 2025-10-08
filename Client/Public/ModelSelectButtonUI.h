#pragma once

#include "Client_Defines.h"
#include "Button.h"

NS_BEGIN(Engine)
class CFont;
NS_END

NS_BEGIN(Client)

class CModelSelectButtonUI : public CButton
{
private:
	CModelSelectButtonUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CModelSelectButtonUI(const CModelSelectButtonUI& rhs);
	virtual ~CModelSelectButtonUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CFont* m_pFontCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CModelSelectButtonUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END
