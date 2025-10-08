#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CFont;
NS_END 

NS_BEGIN(Client)

class CCreateCharacterUI final : public CUIObject
{
private:
	CCreateCharacterUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CCreateCharacterUI(const CCreateCharacterUI& rhs);
	virtual ~CCreateCharacterUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CFont* m_pFontCom = { nullptr };
	_bool m_IsTriggered = { false };
	_float m_fAnimationDist = { 10.f };
	_float m_fTimeAcc = { 0.f };
	_float m_fMaxTimeAcc = { 0.5f };

private:
	void	Play_Animation_FadeIn(_float fTimeDelta);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	
public:
	static CCreateCharacterUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override; 
};

NS_END
