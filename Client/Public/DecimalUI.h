#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer;
class CShader;
NS_END

NS_BEGIN(Client)

class CDecimalUI final : public CUIObject
{
private:
	CDecimalUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CDecimalUI(const CDecimalUI& rhs);
	virtual ~CDecimalUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_CurrentIdx(_uint iIdx) {
		m_iCurrentIndex = iIdx;
	}

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer* m_pVIBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	/* 로직에 따라 바뀔 값. 0~9 사이만을 가짐. */
	_uint m_iCurrentIndex = {};
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
public:
	static CDecimalUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
