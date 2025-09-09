#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CEnemy abstract : public CGameObject
{
protected:
	CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CEnemy(const CEnemy& rhs);
	virtual ~CEnemy() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	HRESULT Ready_HPBar();

protected:
	_float m_fCurrentHP = { 100 };
	_float m_fMaxHP = { 200 };
	class CEnemy_HPBar* m_pHPBar = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override = 0;
};

NS_END
