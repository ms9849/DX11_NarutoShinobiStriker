#pragma once

#include "Component.h"

/*
빌보드 기능이 필요할 경우 해당 컴포넌트를 사용한다.
GameInstance에 캐싱해둔 
1. 카메라의 월드행렬 역행렬 (뷰행렬),
2. 카메라의 월드행렬

두 개를 모두 가져와 세팅해줄 수 있게 한다.
*/
NS_BEGIN(Engine)

class ENGINE_DLL CBillBoard final : public CComponent
{
private:
	CBillBoard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBillBoard(const CBillBoard& Prototype);
	virtual ~CBillBoard() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

private:
	/* 카메라의 월드 행렬과 뷰 행렬 받아오게 한다. 카메라 만들고 나면 바로 세팅해줄 것. */
	_float4x4 m_matCameraWorld{}, m_matView{};

public:
	static CBillBoard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END
