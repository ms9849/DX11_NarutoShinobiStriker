#pragma once

/* 테스트용 객체. 여러 기능들 집어넣고 테스트 해볼 예정 */

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CPooling;
NS_END

NS_BEGIN(Client)

class CDummy final : public CGameObject 
{
private:
	CDummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummy(const CDummy& Prototype);
	virtual ~CDummy() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CPooling* m_pPoolingCom = {};
	_float	  m_fLife = { 0.f };

private:
	HRESULT Ready_Components();

public:
	static CDummy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
