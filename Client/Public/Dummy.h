#pragma once

/* 테스트용 객체. 여러 기능들 집어넣고 테스트 해볼 예정 */

/*
250728 기준으로 더미에 풀링 적용해놓은 상태.

사용법 
0. 원형 로더에서 올려놓긴 할 것
1. 객체에 풀링 컴포넌트 추가 
2. 객체들 Add_GameObject_ToLayer가 아니라 Add_GameObject_ToPool 로 레벨에서 세팅해줄 것
3. 원할때 원하는 태그로 Add_PoolingObject_ToLayer 함수로 오브젝트 매니저에 넣어 줄 것.
4. 객체 내부에서 m_isDead만 세팅해주면 알아서 오브젝트 매니저에 넣어주고, 빠져나오는 작동을 수행해줌.
*/

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
