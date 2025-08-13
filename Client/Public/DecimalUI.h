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
public:
	enum class DECIMAL_TYPE { TIMER, COMBO, END };

	typedef struct tagDecimalUI : public UIOBJECT_DESC {
		DECIMAL_TYPE eDecimal;
	} DECIMAL_DESC;

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
		m_iTextureIdx = ENUM_CLASS(m_eDecimalType) * 10 + iIdx;
	}

	void Set_Visible(_bool bFlag) {
		m_bVisible = bFlag;
	}

private:
	class CGameManager* m_pGameManager = { nullptr };
	DECIMAL_TYPE m_eDecimalType = {};
	_bool m_bVisible = { true };

private:
	HRESULT Ready_Components();
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CDecimalUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
