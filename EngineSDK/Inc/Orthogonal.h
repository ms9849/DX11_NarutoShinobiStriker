#pragma once

#include "Component.h"
/* 
직교 투영을 위한 컴포넌트. UI 뿐만 아니라 
여러 이펙트, 파티클 등에도 활용할 수 있을 것이다.
*/

NS_BEGIN(Engine)

class ENGINE_DLL COrthogonal final : public CComponent
{
private:
	COrthogonal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COrthogonal(const COrthogonal& Prototype);
	virtual ~COrthogonal() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);

public:
	void Orthogonal_Begin();
	void Orthogonal_End();

private:
	/* 직교 투영만을 위한 세팅 */
	_float4x4				m_matView{}, m_matProj = {};
	_float4x4				m_OldViewMatrix{}, m_OldProjMatrix{};

public:
	static COrthogonal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
