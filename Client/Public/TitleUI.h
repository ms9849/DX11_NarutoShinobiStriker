#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CVIBuffer;
class CTexture;
class CShader;
NS_END 

NS_BEGIN(Client)

class CTitleUI final : public CUIObject
{
private:
	CTitleUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CTitleUI(const CTitleUI& rhs);
	virtual ~CTitleUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CTitleUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
