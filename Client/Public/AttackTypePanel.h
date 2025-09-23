#pragma once

#include "Client_Defines.h"
#include "Skill_Table.h"
#include "Panel.h"

NS_BEGIN(Engine)
class CFont;
NS_END

NS_BEGIN(Client)

class CAttackTypePanel : public CPanel
{
private:
	CAttackTypePanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CAttackTypePanel(const CAttackTypePanel& rhs);
	virtual ~CAttackTypePanel() = default;

public:
	void Change_AttackType(ATTACK_TYPE eAttackType);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_AttackType_Visible(_bool bFlag);

private:
	/* 애니메이션 재생 및 플레이 끝남을 확인하기 위함 .*/
	_bool m_IsTriggered = { false };
	_float m_fTimeAcc = { 0.f };
	_float m_fMaxTimeAcc = { 0.5f };
	_float m_fAnimationDist = { 30.f };

	CFont* m_pFontCom = { nullptr };
	_wstring m_strFontText = {};
	_bool   m_IsVisible = { true };

private:
	virtual HRESULT Bind_ShaderResources() override;
	HRESULT Ready_Components();
	HRESULT Ready_Texts();

private:
	void Play_Animation_FadeIn(_float fTimeDelta);

public:
	static CAttackTypePanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
