#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Client)

/* 
고유한 아이디를 가지긴 하지만 내부에서 상태가 달라짐..
이걸 세팅해주기 위해선 로딩할 떄 다른 방식이 필요할 것이다.
*/
class CProps : public CGameObject
{
private:
	CProps(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	CProps(const CProps& rhs);
	virtual ~CProps() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint m_iMeshNum;

public:
	static CProps* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJECTID eObjectID);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END
