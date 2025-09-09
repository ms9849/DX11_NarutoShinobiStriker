#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

/*
직교 투영하지 않는 HP바.
몬스터가 들고 있고,

직교투영을 수행하지 않고 몬스터를 따라다니면서 
체력을 표시해주므로,
UIObject 계열로 두지 않고 따로 구현한다.
*/

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CEnemy_HPBar : public CGameObject
{
public:
	typedef struct tagEnemyHPBar {
		class CTransform* pTargetTransform = {};
	} ENEMY_HPBAR_DESC;
private:
	CEnemy_HPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CEnemy_HPBar(const CEnemy_HPBar& rhs);
	virtual ~CEnemy_HPBar() = default;

public:
	void Set_HP(_float fCurrentHP, _float fMaxHP);
public:

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CTransform* m_pTargetTransform = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };

	_float m_fCurrentHp = 50.f;
	_float m_fMaxHp = 100.f;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEnemy_HPBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CEnemy_HPBar* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
