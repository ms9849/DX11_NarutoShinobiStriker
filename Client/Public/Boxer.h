#pragma once

#include "Client_Defines.h"
#include "Enemy.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CNavigation;
NS_END

NS_BEGIN(Client)

class CBoxer final : public CEnemy
{
private:
	CBoxer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CBoxer(const CBoxer& rhs);
	virtual ~CBoxer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	/* 추후 상태 추가해야함*/
	//class CWhiteJetsuState* m_pState = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	void	Update_State(_float fTimeDelta);

public:
	static CBoxer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
