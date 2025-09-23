#pragma once

#include "Client_Defines.h"
#include "Panel.h"

/* 
콤보는 최대 99까지. 
1 콤보 -> 큰 HIT로 땜빵
2 콤보 이상 -> Decimal UI 가져다 써야할 듯

KO -> 별개로 적 처치시에 띄워줄 거니까.. UI 별개로 필요함. (KO랑 콤보랑은 다름)

Hit, Hits는 ComboPanel에서 출력.
숫자는 Decimal.

KO는 KO UI 에서.
*/

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer;
class CShader;
NS_END

NS_BEGIN(Client)

class CComboKOPanel final : public CPanel
{
private:
	CComboKOPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CComboKOPanel(const CComboKOPanel& rhs);
	virtual ~CComboKOPanel() = default;

public:
	void PopUp_KO();
	void Active_Combo();
	void Active_KO();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint	m_iPreComboCount = { 0 };
	_uint	m_iComboCount = { 0 };
	_uint	m_iMaxComboCount = { 99 };

	_float	m_fComboTimeAcc = { 0 };
	_bool	m_IsEnemyHit = { false };
	_bool	m_IsEnemyKO = { false };

	_float	m_fScale = { 1.f };

	_bool	m_IsVisible = { false };
	_float	m_fMaxScale = { 1.5f };

	/* 페이드 인*/
	_bool	m_IsFadeIn = { false };
	_float	m_fFadeInTimeAcc = { 0.f };
	_float	m_fFadeInMaxTimeAcc = { 0.2f };

	/* 페이드 아웃 */
	_bool  m_IsFadeOut = { false };
	_float m_fFadeOutTimeAcc = { 0.f };
	_float m_fFadeOutMaxTimeAcc = { 0.2f };

private:
	virtual HRESULT Bind_ShaderResources() override;
	HRESULT Ready_Components();
	HRESULT Ready_Deicmals();
	HRESULT Ready_KO();

private:
	void	Play_Animation_FadeIn(_float fTimeDelta);
	void	Play_Animation_FadeOut(_float fTimeDelta);
	void	ComboKO_System(_float fTimeDelta);
	void	Update_Combo();

public:
	static CComboKOPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END